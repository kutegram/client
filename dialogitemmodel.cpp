#include "dialogitemmodel.h"

#include "library/telegramclient.h"
#include <QPixmap>

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), messages(), chats(), users(), avatars(), client(cl), requestLock(QMutex::Recursive), offsetId(), offsetDate(), offsetPeer(), gotFull(false)
{
    connect(client, SIGNAL(gotDialogs(qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)), this, SLOT(client_gotDialogs(qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)));
    connect(client, SIGNAL(gotFile(qint64,TLType::Types,qint32,QByteArray)), this, SLOT(client_gotFile(qint64,TLType::Types,qint32,QByteArray)));
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
    TLDialog d = dialogs[i];

    switch (d.peer.type) {
    case TLType::PeerChat:
    case TLType::PeerChannel:
    {
        return chats[d.peer.id].title;
    }
    case TLType::PeerUser:
    {
        return users[d.peer.id].firstName + " " + users[d.peer.id].lastName;
    }
    }

    return QString();
}

qint32 DialogItemModel::getDialogId(qint32 i) const
{
    return dialogs[i].peer.id;
}

QVariant DialogItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    //TODO
    switch (role) {
    case Qt::DisplayRole: //title
    {
        return getDialogTitle(index.row());
    }
    case Qt::DecorationRole: //avatar
    {
        return avatars[getDialogId(index.row())];
    }
    case Qt::ToolTipRole: //last message
    {
        return QVariant();
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

void DialogItemModel::fetchMore(const QModelIndex &parent)
{
    if (!requestLock.tryLock()) return;

    client->getDialogs(offsetDate, offsetId, offsetPeer, 40);

    requestLock.unlock();
}

void DialogItemModel::client_gotDialogs(qint32 count, QList<TLDialog> d, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u)
{
    requestLock.lock();
    beginInsertRows(QModelIndex(), dialogs.size(), dialogs.size() + d.size() - 1);

    if (!count) gotFull = true;
    else gotFull = (d.count() != 40);
    dialogs.append(d);

    for (qint32 i = 0; i < m.size(); ++i) messages.insert(m[i].id, m[i]);
    for (qint32 i = 0; i < c.size(); ++i) {
        TLChat item = c[i];
        if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false)));
        chats.insert(item.id, item);
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TLUser item = u[i];
        if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false)));
        users.insert(item.id, item);
    }

    for (qint32 i = 0; i < d.size(); ++i) {
        TLMessage topMessage = messages[d[i].topMessage];
        if (!offsetDate || topMessage.date < offsetDate) {
            offsetDate = topMessage.date;
            offsetId = topMessage.id;

            switch (topMessage.peer.type) {
            case TLType::PeerChat:
            case TLType::PeerChannel:
                offsetPeer = TLInputPeer(chats[topMessage.peer.id]);
                break;
            case TLType::PeerUser:
                offsetPeer = TLInputPeer(users[topMessage.peer.id]);
                break;
            }


        }
    }

    endInsertRows();
    requestLock.unlock();
}

void DialogItemModel::client_gotFile(qint64 mtMessageId, TLType::Types type, qint32 mtime, QByteArray bytes)
{
    avatars.insert(avatars.key(mtMessageId), QPixmap::fromImage(QImage::fromData(bytes)));
}
