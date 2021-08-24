#include "dialogitemmodel.h"

#include "library/telegramclient.h"

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), client(cl), canRequest(true)
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

QVariant DialogItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    //TODO
    switch (role) {
    case Qt::DisplayRole:
    {
        return QString::number(dialogs[index.row()].peer.id);
    }
    case Qt::DecorationRole:
    {
        break;
    }
    case Qt::ToolTipRole:
    {
        break;
    }
    }

    return QVariant();
}

bool DialogItemModel::canFetchMore(const QModelIndex &parent) const
{
    return client->isConnected() && client->isLoggedIn() && canRequest;
}

void DialogItemModel::fetchMore(const QModelIndex &parent)
{
    //TODO
    if (!canRequest) return;
    canRequest = false;
    client->getDialogs();
}

void DialogItemModel::client_gotDialogs(qint32 count, QList<TLDialog> d, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u)
{
    beginInsertRows(QModelIndex(), dialogs.size(), dialogs.size() + d.size() - 1);
    dialogs.append(d);
    canRequest = true;
    endInsertRows();
}
