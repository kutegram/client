#include "historyitemmodel.h"

#include "library/telegramclient.h"
#include <QPixmap>
#include <QApplication>
#include <QFontMetrics>
#include "avatars.h"

HistoryItemModel::HistoryItemModel(TelegramClient *cl, TLInputPeer input, QObject *parent) :
    QAbstractItemModel(parent),
    history(),
    messages(),
    users(),
    chats(),
    thumbnails(),
    client(cl),
    requestLock(QMutex::Recursive),
    gotFull(),
    peer(input),
    offsetId(),
    offsetDate(),
    requestId(),
    replyRequestId(),
    replies()
{
    connect(client, SIGNAL(gotMessages(qint64,qint32,QList<TLMessage>,QList<TLChat>,QList<TLUser>,qint32,qint32,bool)), this, SLOT(client_gotMessages(qint64,qint32,QList<TLMessage>,QList<TLChat>,QList<TLUser>,qint32,qint32,bool)));
}

QModelIndex HistoryItemModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column);
}

int HistoryItemModel::rowCount(const QModelIndex& parent) const
{
    return history.size();
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
    TLMessage m = messages[history[index.row()]];

    switch (role) {
    case Qt::DisplayRole:
    {
        return QVariant::fromValue(m);
    }
    case Qt::DecorationRole:
    {
        return thumbnails[m.from.id];
    }
    case Qt::UserRole:
    {
        TLPeer from = m.from;
        switch (from.type) {
        case TLType::PeerChannel:
        case TLType::PeerChat:
            return QVariant::fromValue(chats[from.id]);
        case TLType::PeerUser:
            return QVariant::fromValue(users[from.id]);
        }
    }
    case Qt::ToolTipRole:
    {
        if (m.reply.type)
            return QVariant::fromValue(messages[m.reply.msgId]);
    }
    case Qt::ToolTipPropertyRole:
    {
        if (m.reply.type) {
            TLPeer from = messages[m.reply.msgId].from;
            switch (from.type) {
            case TLType::PeerChannel:
            case TLType::PeerChat:
                return QVariant::fromValue(chats[from.id]);
            case TLType::PeerUser:
                return QVariant::fromValue(users[from.id]);
            }
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
    if (requestId || replyRequestId) {
        requestLock.unlock();
        return;
    }

    requestId = client->getHistory(peer, offsetId, offsetId, 0, 40);

    requestLock.unlock();
}

void HistoryItemModel::gotHistoryMessages(qint64 mtm, qint32 count, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= (m.count() != 40);

    qint32 fH = QApplication::fontMetrics().height();

    for (qint32 i = 0; i < c.size(); ++i) {
        TLChat item = c[i];
        chats.insert(item.id, item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.title, fH + fH));
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TLUser item = u[i];
        users.insert(item.id, item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.firstName + " " + item.lastName, fH + fH));
    }

    beginInsertRows(QModelIndex(), 0, m.size() - 1);

    for (qint32 i = 0; i < m.size(); ++i) {
        TLMessage msg = m[i];
        history.insert(0, msg.id);
        messages.insert(msg.id, msg);
        if (!offsetDate || msg.date < offsetDate) {
            offsetDate = msg.date;
            offsetId = msg.id;
        }
    }

    endInsertRows();

    QList<TLInputMessage> notFoundReplies;
    for (qint32 i = 0; i < m.size(); ++i) {
        TLMessage reply = m[i];
        if (!reply.reply.type || history.contains(reply.reply.msgId)) continue;

        TLInputMessage input;
        input.type = TLType::InputMessageID;
        input.id = reply.reply.msgId;
        notFoundReplies.append(input);
        replies.append(reply.id);
    }

    if (!notFoundReplies.isEmpty())
        replyRequestId = client->getMessages(notFoundReplies);
}

void HistoryItemModel::gotReplyMessages(qint64 mtm, qint32 count, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    replyRequestId = 0;

    for (qint32 i = 0; i < m.size(); ++i) {
        TLMessage msg = m[i];
        messages.insert(msg.id, msg);
    }

    for (qint32 i = 0; i < c.size(); ++i) {
        TLChat item = c[i];
        chats.insert(item.id, item);
    }

    for (qint32 i = 0; i < u.size(); ++i) {
        TLUser item = u[i];
        users.insert(item.id, item);
    }

    for (qint32 i = 0; i < replies.size(); ++i) {
        QModelIndex item = index(history.indexOf(replies[i]), 0);
        emit dataChanged(item, item);
    }
    replies.clear();
}

void HistoryItemModel::client_gotMessages(qint64 mtm, qint32 count, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    requestLock.lock();

    if (mtm == replyRequestId) {
        gotReplyMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
    } else if (mtm == requestId) {
        gotHistoryMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
    }

    requestLock.unlock();
}
