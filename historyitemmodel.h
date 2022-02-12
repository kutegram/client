#ifndef HISTORYITEMMODEL_H
#define HISTORYITEMMODEL_H

#include <QAbstractItemModel>
#include <QMutex>
#include "tl.h"
#include <QHash>
#include <QPixmap>

class TelegramClient;

class HistoryItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<qint32> history;
    QList<qint32> replies;
    QHash<qint32, TObject> messages;
    QHash<qint64, TObject> chats;
    QHash<qint64, TObject> users;
    QHash<qint64, QPixmap> usersThumbnails;
    QHash<qint64, QPixmap> chatsThumbnails;
    TelegramClient* client;
    TObject peer;
    QMutex requestLock;
    bool gotFull;
    qint32 offsetId;
    qint32 offsetDate;
    qint64 requestId;
    qint64 replyRequestId;

    explicit HistoryItemModel(TelegramClient* cl, TObject input, QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool canFetchMore(const QModelIndex& parent) const;
    virtual void fetchMore(const QModelIndex& parent);

    virtual bool canFetchMoreUpwards(const QModelIndex& parent) const;
    virtual void fetchMoreUpwards(const QModelIndex& parent);

    QString getMessageString(qint32 i) const;
signals:
    
public slots:
    void client_gotMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    void gotHistoryMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    void gotReplyMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
};

#endif // HISTORYITEMMODEL_H
