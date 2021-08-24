#ifndef DIALOGITEMMODEL_H
#define DIALOGITEMMODEL_H

#include <QAbstractItemModel>
#include "library/tlmessages.h"

class TelegramClient;

class DialogItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<TLDialog> dialogs;
    TelegramClient* client;
    bool canRequest;

    explicit DialogItemModel(TelegramClient* cl, QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool canFetchMore(const QModelIndex& parent) const;
    virtual void fetchMore (const QModelIndex& parent);

signals:

public slots:
    void client_gotDialogs(qint32 count, QList<TLDialog> dialogs, QList<TLMessage> messages, QList<TLChat> chats, QList<TLUser> users);
};

#endif // DIALOGITEMMODEL_H
