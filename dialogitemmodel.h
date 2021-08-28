#ifndef DIALOGITEMMODEL_H
#define DIALOGITEMMODEL_H

#include <QAbstractItemModel>
#include <QMutex>
#include "library/tlmessages.h"

class TelegramClient;

class DialogItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<TLDialog> dialogs;
    QMap<qint32, TLMessage> messages;
    QMap<qint32, TLChat> chats;
    QMap<qint32, TLUser> users;
    TelegramClient* client;
    QMutex requestLock;
    qint32 offsetId;
    qint32 offsetDate;
    TLInputPeer offsetPeer;
    bool gotFull;

    explicit DialogItemModel(TelegramClient* cl, QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool canFetchMore(const QModelIndex& parent) const;
    virtual void fetchMore(const QModelIndex& parent);

    QString getDialogTitle(qint32 i) const;
    qint32 getDialogId(qint32 i) const;
signals:

public slots:
    void client_gotDialogs(qint32 count, QList<TLDialog> dialogs, QList<TLMessage> messages, QList<TLChat> chats, QList<TLUser> users);
};

#endif // DIALOGITEMMODEL_H
