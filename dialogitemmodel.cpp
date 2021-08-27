#include "dialogitemmodel.h"

#include "library/telegramclient.h"

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), messages(), chats(), users(), client(cl), requestLock(QMutex::Recursive)
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

QVariant DialogItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    //TODO
    switch (role) {
    case Qt::DisplayRole:
    {
        return getDialogTitle(index.row());
    }
    case Qt::DecorationRole:
    {
        return QVariant();
    }
    case Qt::ToolTipRole:
    {
        return QVariant();
    }
    }

    return QVariant();
}

bool DialogItemModel::canFetchMore(const QModelIndex &parent) const
{
    return client->apiReady();
}

void DialogItemModel::fetchMore(const QModelIndex &parent)
{
    //TODO
    if (!requestLock.tryLock()) return;

    client->getDialogs();

    requestLock.unlock();
}

void DialogItemModel::client_gotDialogs(qint32 count, QList<TLDialog> d, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u)
{
    //TODO
    requestLock.lock();
    beginInsertRows(QModelIndex(), dialogs.size(), dialogs.size() + d.size() - 1);

    dialogs.append(d);

    for (qint32 i = 0; i < m.size(); ++i) messages.insert(m[i].id, m[i]);
    for (qint32 i = 0; i < c.size(); ++i) chats.insert(c[i].id, c[i]);
    for (qint32 i = 0; i < u.size(); ++i) users.insert(u[i].id, u[i]);

    endInsertRows();
    requestLock.unlock();
}
