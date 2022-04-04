#include "dialogitemmodel.h"

#include "avatars.h"
#include "telegramclient.h"
#include <QFontMetrics>
#include <QApplication>
#include "tlschema.h"

using namespace TLType;

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), messages(), chats(), users(), usersThumbnails(), chatsThumbnails(), client(cl), requestLock(QMutex::Recursive), offsetId(), offsetDate(), offsetPeer(), gotFull(), requestId()
{
    connect(client, SIGNAL(gotDialogs(qint64,qint32,TVector,TVector,TVector,TVector)), this, SLOT(client_gotDialogs(qint64,qint32,TVector,TVector,TVector,TVector)));
}

QModelIndex DialogItemModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

int DialogItemModel::rowCount(const QModelIndex &parent) const
{
    return dialogs.size();
}

int DialogItemModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QModelIndex DialogItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

QString DialogItemModel::getDialogTitle(qint32 i) const
{
    TObject peer = dialogs[i]["peer"].toMap();
    qint64 pid = getPeerId(peer).toLongLong();

    switch (ID(peer)) {
    case TLType::PeerChat:
    case TLType::PeerChannel:
        return chats[pid]["title"].toString();
    case TLType::PeerUser:
        return users[pid]["first_name"].toString() + " " + users[pid]["last_name"].toString();
    }

    return QString();
}

TObject DialogItemModel::getInputPeerByIndex(qint32 i) const
{
    TObject peer = dialogs[i]["peer"].toMap();
    qint64 pid = getPeerId(peer).toLongLong();

    switch (ID(peer)) {
    case TLType::PeerChat:
    case TLType::PeerChannel:
        return getInputPeer(chats[pid]);
    case TLType::PeerUser:
        return getInputPeer(users[pid]);
    }

    return TObject();
}

QString DialogItemModel::getMessageString(qint32 i) const
{
    TObject m = messages[dialogs[i]["top_message"].toInt()];

    if (!m["message"].toString().isEmpty())
        return m["message"].toString();
    else if (GETID(m["action"].toMap()))
        return m["action"].toString();
    else if (GETID(m["media"].toMap()))
        return m["media"].toString();
    else return QVariant(m).toString();
}

qint64 DialogItemModel::getDialogId(qint32 i) const
{
    return getPeerId(dialogs[i]["peer"].toMap()).toLongLong();
}

QVariant DialogItemModel::data(const QModelIndex &index, int role) const
{
    //TODO
    switch (role) {
    case Qt::DisplayRole: //title
    {
        return getDialogTitle(index.row());
    }
    case Qt::DecorationRole: //avatar
    {
        TObject from = dialogs[index.row()]["peer"].toMap();

        switch (ID(from)) {
        case TLType::PeerChannel:
        case TLType::PeerChat:
            return chatsThumbnails[getPeerId(from).toLongLong()];
        case TLType::PeerUser:
            return usersThumbnails[getPeerId(from).toLongLong()];
        }
    }
    case Qt::ToolTipRole: //last message
    {
        return getMessageString(index.row());
    }
    case Qt::UserRole: //dialog id
    {
        return getDialogId(index.row());
    }
    }

    return QVariant();
}

bool DialogItemModel::canFetchMore(const QModelIndex &parent) const
{
    return client->apiReady() && !gotFull;
}

//TODO: FIXME: first and second request have duplicate results
void DialogItemModel::fetchMore(const QModelIndex &parent)
{
    if (!requestLock.tryLock()) return;
    if (requestId) return;

    requestId = client->getDialogs(offsetDate, offsetId, offsetPeer, 40);

    requestLock.unlock();
}

void DialogItemModel::client_gotDialogs(qint64 mtm, qint32 count, TVector d, TVector m, TVector c, TVector u)
{
    requestLock.lock();
    if (requestId != mtm) {
        requestLock.unlock();
        return;
    }

    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= d.isEmpty();

    for (qint32 i = 0; i < m.size(); ++i)
        messages.insert(m[i].toMap()["id"].toInt(), m[i].toMap());

    qint32 fH = QApplication::fontMetrics().height();
    fH += fH;
    fH += 12;

    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        chatsThumbnails.insert(item["id"].toLongLong(), generateThumbnail(item["id"].toLongLong(), item["title"].toString(), fH));
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        usersThumbnails.insert(item["id"].toLongLong(), generateThumbnail(item["id"].toLongLong(), item["first_name"].toString() + " " + item["last_name"].toString(), fH));
    }

    if (!d.isEmpty()) {
        beginInsertRows(QModelIndex(), dialogs.size(), dialogs.size() + d.size() - 1);

        for (qint32 i = 0; i < d.size(); ++i) {
            TObject item = d[i].toMap();
            TObject topMessage = messages[item["top_message"].toInt()];
            if (!offsetDate || topMessage["date"].toInt() < offsetDate) {
                offsetDate = topMessage["date"].toInt();
                offsetId = topMessage["id"].toInt();

                switch (ID(topMessage["peer_id"].toMap())) {
                case TLType::PeerChat:
                case TLType::PeerChannel:
                    offsetPeer = getInputPeer(chats[getPeerId(topMessage["peer_id"].toMap()).toLongLong()]);
                    break;
                case TLType::PeerUser:
                    offsetPeer = getInputPeer(users[getPeerId(topMessage["peer_id"].toMap()).toLongLong()]);
                    break;
                }
            }

            dialogs << item;
        }

        endInsertRows();
    }

    requestLock.unlock();
}
