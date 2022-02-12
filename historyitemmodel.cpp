#include "historyitemmodel.h"

#include "library/telegramclient.h"
#include <QPixmap>
#include <QApplication>
#include <QFontMetrics>
#include "avatars.h"
#include "tlschema.h"

using namespace TLType;

HistoryItemModel::HistoryItemModel(TelegramClient *cl, TObject input, QObject *parent) :
    QAbstractItemModel(parent),
    history(),
    messages(),
    users(),
    chats(),
    usersThumbnails(),
    chatsThumbnails(),
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
    connect(client, SIGNAL(gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)), this, SLOT(client_gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)));
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
    TObject m = messages[history[index.row()]];

    switch (role) {
    case Qt::DisplayRole:
    {
        return m;
    }
    case Qt::DecorationRole:
    {
        TObject from = m["from_id"].toMap();
        switch (ID(from)) {
        case TLType::PeerChannel:
        case TLType::PeerChat:
            return chatsThumbnails[getPeerId(from).toLongLong()];
        case TLType::PeerUser:
            return usersThumbnails[getPeerId(from).toLongLong()];
        }
    }
    case Qt::UserRole:
    {
        TObject from = m["from_id"].toMap();
        switch (ID(from)) {
        case TLType::PeerChannel:
        case TLType::PeerChat:
            return chats[getPeerId(from).toLongLong()];
        case TLType::PeerUser:
            return users[getPeerId(from).toLongLong()];
        }
    }
    case Qt::ToolTipRole:
    {
        if (ID(m["reply_to"].toMap()))
            return messages[m["reply_to"].toMap()["reply_to_msg_id"].toInt()];
    }
    case Qt::ToolTipPropertyRole:
    {
        if (ID(m["reply_to"].toMap())) {
            TObject from = messages[m["reply_to"].toMap()["reply_to_msg_id"].toInt()]["from_id"].toMap();
            switch (ID(from)) {
            case TLType::PeerChannel:
            case TLType::PeerChat:
                return chats[getPeerId(from).toLongLong()];
            case TLType::PeerUser:
                return users[getPeerId(from).toLongLong()];
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

void HistoryItemModel::gotHistoryMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= (m.count() != 40);

    qint32 fH = QApplication::fontMetrics().height();

    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        chatsThumbnails.insert(item["id"].toLongLong(), Avatars::generateThumbnail(item["id"].toLongLong(), item["title"].toString(), fH + fH));
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        usersThumbnails.insert(item["id"].toLongLong(), Avatars::generateThumbnail(item["id"].toLongLong(), item["first_name"].toString() + " " + item["last_name"].toString(), fH + fH));
    }

    beginInsertRows(QModelIndex(), 0, m.size() - 1);

    for (qint32 i = m.size() - 1; i >= 0; --i) {
        TObject msg = m[i].toMap();
        history.append(msg["id"].toInt());
        messages.insert(msg["id"].toInt(), msg);
        if (!offsetDate || msg["date"].toInt() < offsetDate) {
            offsetDate = msg["date"].toInt();
            offsetId = msg["id"].toInt();
        }
    }

    endInsertRows();

    TVector notFoundReplies;
    for (qint32 i = 0; i < m.size(); ++i) {
        TObject reply = m[i].toMap();
        if (!ID(reply["reply_to"].toMap()) || history.contains(reply["reply_to"].toMap()["reply_to_msg_id"].toInt())) continue;

        TObject input = getInputMessage(reply["reply_to"].toMap());
        notFoundReplies.append(input);
        replies.append(reply["id"].toInt());
    }

    if (!notFoundReplies.isEmpty())
        replyRequestId = client->getMessages(notFoundReplies);
}

void HistoryItemModel::gotReplyMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    replyRequestId = 0;

    for (qint32 i = 0; i < m.size(); ++i) {
        TObject msg = m[i].toMap();
        messages.insert(msg["id"].toInt(), msg);
    }

    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
    }

    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
    }

    for (qint32 i = 0; i < replies.size(); ++i) {
        QModelIndex item = index(history.indexOf(replies[i]), 0);
        emit dataChanged(item, item);
    }
    replies.clear();
}

void HistoryItemModel::client_gotMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    requestLock.lock();

    if (mtm == replyRequestId) {
        gotReplyMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
    } else if (mtm == requestId) {
        gotHistoryMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
    }

    requestLock.unlock();
}
