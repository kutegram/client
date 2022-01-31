#ifndef DIALOGITEMMODEL_H
#define DIALOGITEMMODEL_H

#include <QAbstractItemModel>
#include <QMutex>
#include <QPixmap>
#include "library/tlmessages.h"
#include <QHash>

class TelegramClient;

class DialogItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QList<TLDialog> dialogs;
    QHash<qint64, TLMessage> messages;
    QHash<qint64, TLChat> chats;
    QHash<qint64, TLUser> users;
    QHash<qint64, QVariant> avatars;
    QHash<qint64, QPixmap> thumbnails;
    TelegramClient* client;
    QMutex requestLock;
    qint32 offsetId;
    qint32 offsetDate;
    TLInputPeer offsetPeer;
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
    TLInputPeer getInputPeer(qint32 i) const;
signals:

public slots:
    void client_gotDialogs(qint64 mtm, qint32 count, QList<TLDialog> dialogs, QList<TLMessage> messages, QList<TLChat> chats, QList<TLUser> users);
    void client_gotFilePart(qint64 mtMessageId, TLType::Types type, qint32 mtime, QByteArray bytes);
};

#endif // DIALOGITEMMODEL_H
