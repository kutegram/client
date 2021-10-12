#ifndef HISTORYITEMMODEL_H
#define HISTORYITEMMODEL_H

#include <QAbstractItemModel>
#include <QMutex>
#include "library/tlmessages.h"

class TelegramClient;

class HistoryItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<TLMessage> messages;
    QList<TLChat> chats;
    QList<TLUser> users;
    TelegramClient* client;
    TLInputPeer peer;
    QMutex requestLock;
    bool gotFull;
    qint32 offsetId;
    qint32 offsetDate;
    qint64 requestId;

    explicit HistoryItemModel(TelegramClient* cl, TLInputPeer input, QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool canFetchMore(const QModelIndex& parent) const;
    virtual void fetchMore(const QModelIndex& parent);

    QString getMessageString(qint32 i) const;
signals:
    
public slots:
    void client_gotMessages(qint64 mtm, qint32 count, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    
};

#endif // HISTORYITEMMODEL_H
