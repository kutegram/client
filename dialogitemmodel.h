#ifndef DIALOGITEMMODEL_H
#define DIALOGITEMMODEL_H

#include <QAbstractItemModel>
#include <QMutex>
#include <QPixmap>
#include "tl.h"
#include <QHash>

class TelegramClient;

class DialogItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<TObject> dialogs;
    QHash<qint64, TObject> messages;
    QHash<qint64, TObject> chats;
    QHash<qint64, TObject> users;
    QHash<qint64, QPixmap> usersThumbnails;
    QHash<qint64, QPixmap> chatsThumbnails;
    TelegramClient* client;
    QMutex requestLock;
    qint32 offsetId;
    qint32 offsetDate;
    TObject offsetPeer;
    bool gotFull;
    qint64 requestId;

    explicit DialogItemModel(TelegramClient* cl, QObject *parent = 0);
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& index) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual bool canFetchMore(const QModelIndex& parent) const;
    virtual void fetchMore(const QModelIndex& parent);

    QString getDialogTitle(qint32 i) const;
    QString getMessageString(qint32 i) const;
    qint64 getDialogId(qint32 i) const;
    TObject getInputPeerByIndex(qint32 i) const;
signals:

public slots:
    void client_gotDialogs(qint64 mtm, qint32 count, TVector dialogs, TVector messages, TVector chats, TVector users);
};

#endif // DIALOGITEMMODEL_H
