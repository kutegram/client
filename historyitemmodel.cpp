#include "historyitemmodel.h"

#include "library/telegramclient.h"
#include <QPixmap>
#include <QApplication>
#include <QFontMetrics>
#include "avatars.h"

HistoryItemModel::HistoryItemModel(TelegramClient *cl, TLInputPeer input, QObject *parent) :
    QAbstractItemModel(parent), messages(), users(), chats(), client(cl), requestLock(QMutex::Recursive), gotFull(), peer(input), offsetId(), offsetDate(), requestId(), thumbnails()
{
    connect(client, SIGNAL(gotMessages(qint64,qint32,QList<TLMessage>,QList<TLChat>,QList<TLUser>,qint32,qint32,bool)), this, SLOT(client_gotMessages(qint64,qint32,QList<TLMessage>,QList<TLChat>,QList<TLUser>,qint32,qint32,bool)));
}

QModelIndex HistoryItemModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column);
}

int HistoryItemModel::rowCount(const QModelIndex& parent) const
{
    return messages.size();
}

int HistoryItemModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QModelIndex HistoryItemModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

QVariant HistoryItemModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        return QVariant::fromValue(messages[index.row()]);
    }
    case Qt::DecorationRole:
    {
        return thumbnails[messages[index.row()].from.id];
    }
    case Qt::UserRole:
    {
        TLMessage m = messages[index.row()];
        switch (m.from.type) {
        case TLType::PeerChannel:
        case TLType::PeerChat:
            return QVariant::fromValue(chats[m.from.id]);
        case TLType::PeerUser:
            return QVariant::fromValue(users[m.from.id]);
        }
    }
    }

    return QVariant();
}

bool HistoryItemModel::canFetchMore(const QModelIndex& parent) const
{
    return false;
}

void HistoryItemModel::fetchMore(const QModelIndex& parent)
{

}

bool HistoryItemModel::canFetchMoreUpwards(const QModelIndex& parent) const
{
    return client->apiReady() && !gotFull;
}

void HistoryItemModel::fetchMoreUpwards(const QModelIndex& parent)
{
    if (!requestLock.tryLock()) return;
    if (requestId) return;

    requestId = client->getHistory(peer, offsetId, offsetId, 0, 40);

    requestLock.unlock();
}

void HistoryItemModel::client_gotMessages(qint64 mtm, qint32 count, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    requestLock.lock();
    if (requestId != mtm) {
        requestLock.unlock();
        return;
    }

    beginInsertRows(QModelIndex(), 0, m.size() - 1);

    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= (m.count() != 40);

    for (qint32 i = 0; i < m.size(); ++i) messages.insert(0, m[i]);
    for (qint32 i = 0; i < c.size(); ++i) chats.insert(c[i].id, c[i]);
    for (qint32 i = 0; i < u.size(); ++i) users.insert(u[i].id, u[i]);

    qint32 fH = QApplication::fontMetrics().height();
    fH += fH;
    fH += 4;

    for (qint32 i = 0; i < c.size(); ++i) {
        TLChat item = c[i];
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.title, fH));
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TLUser item = u[i];
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.firstName + " " + item.lastName, fH));
    }

    for (qint32 i = 0; i < m.size(); ++i) {
        TLMessage msg = m[i];
        if (!offsetDate || msg.date < offsetDate) {
            offsetDate = msg.date;
            offsetId = msg.id;
        }
    }

    endInsertRows();
    requestLock.unlock();
}
