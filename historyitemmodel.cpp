#include "historyitemmodel.h"

#include "library/telegramclient.h"
#include <QPixmap>

HistoryItemModel::HistoryItemModel(TelegramClient *cl, TLInputPeer input, QObject *parent) :
    QAbstractItemModel(parent), messages(), users(), chats(), client(cl), requestLock(QMutex::Recursive), gotFull(), peer(input), offsetId(), offsetDate(), requestId()
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

//TODO Unify
//TODO move users and chats to client inner DB
QString HistoryItemModel::getMessageString(qint32 i) const
{
    TLMessage m = messages[i];

    if (!m.message.isEmpty()) {
        return m.message;
    }
    else if (GETID(m.action)) {
        return "MessageAction#" + QString::number(GETID(m.action));
    }
    else if (GETID(m.media)){
        return "MessageMedia#" + QString::number(GETID(m.media));
    }
    else {
        return "Message#" + QString::number(m.id);
    }
}

QVariant HistoryItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    //TODO
    switch (role) {
    case Qt::DisplayRole: //text
    {
        return getMessageString(index.row());
    }
    case Qt::DecorationRole: //avatar
    {
        return QVariant();
    }
    case Qt::ToolTipRole: //user title
    {
        return QVariant();
    }
    case Qt::UserRole: //message date
    {
        return QVariant();
    }
    }

    return QVariant();
}

bool HistoryItemModel::canFetchMore(const QModelIndex& parent) const
{
    return client->apiReady() && !gotFull;
}

//TODO: FIXME: first and second request have duplicate results
void HistoryItemModel::fetchMore(const QModelIndex& parent)
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

    beginInsertRows(QModelIndex(), messages.size(), messages.size() + m.size() - 1);

    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= (m.count() != 40);

    messages.append(m);
    chats.append(c);
    users.append(u);

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
