#include "dialogitemmodel.h"

#include "library/telegramclient.h"

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), messages(), chats(), users(), client(cl), requestLock(QMutex::Recursive), offsetId(), offsetDate(), offsetPeer(), gotFull(false)
{
    connect(client, SIGNAL(gotDialogs(qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)), this, SLOT(client_gotDialogs(qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)));
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
        return QVariant();
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
    for (qint32 i = 0; i < c.size(); ++i) chats.insert(c[i].id, c[i]);
    for (qint32 i = 0; i < u.size(); ++i) users.insert(u[i].id, u[i]);

    for (qint32 i = 0; i < d.size(); ++i) {
        TLMessage topMessage = messages[d[i].topMessage];
        if (!offsetDate || topMessage.date < offsetDate) {
            offsetDate = topMessage.date;
            offsetId = topMessage.id;

            qint64 accessHash = 0;
            switch (topMessage.peer.type) {
            case TLType::PeerChannel:
                accessHash = chats[topMessage.peer.id].accessHash;
                break;
            case TLType::PeerUser:
                accessHash = users[topMessage.peer.id].accessHash;
                break;
            }

            offsetPeer = TLInputPeer(topMessage.peer, accessHash);
        }
    }

    endInsertRows();
    requestLock.unlock();
}
