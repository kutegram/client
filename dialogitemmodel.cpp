#include "dialogitemmodel.h"

#include "avatars.h"
#include "library/telegramclient.h"
#include <QFontMetrics>
#include <QApplication>

DialogItemModel::DialogItemModel(TelegramClient *cl, QObject *parent) :
    QAbstractItemModel(parent), dialogs(), messages(), chats(), users(), avatars(), thumbnails(), client(cl), requestLock(QMutex::Recursive), offsetId(), offsetDate(), offsetPeer(), gotFull(), requestId()
{
    connect(client, SIGNAL(gotDialogs(qint64,qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)), this, SLOT(client_gotDialogs(qint64,qint32,QList<TLDialog>,QList<TLMessage>,QList<TLChat>,QList<TLUser>)));
    connect(client, SIGNAL(gotFilePart(qint64,TLType::Types,qint32,QByteArray)), this, SLOT(client_gotFilePart(qint64,TLType::Types,qint32,QByteArray)));
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

TLInputPeer DialogItemModel::getInputPeer(qint32 i) const
{
    TLDialog d = dialogs[i];

    switch (d.peer.type) {
    case TLType::PeerChat:
    case TLType::PeerChannel:
        return TLInputPeer(chats[d.peer.id]);
    case TLType::PeerUser:
        return TLInputPeer(users[d.peer.id]);
    }
}

QString DialogItemModel::getMessageString(qint32 i) const
{
    TLMessage m = messages[dialogs[i].topMessage];

    if (!m.message.isEmpty()) {
        return m.message;
    }
    else if (GETID(m.action)) {
        return "MessageAction#" + QString::number(GETID(m.action));
    }
    else if (GETID(m.media)){
        return "MessageMedia#" + QString::number(GETID(m.media));
    }
    else {
        return "Message#" + QString::number(m.id);
    }
}

qint64 DialogItemModel::getDialogId(qint32 i) const
{
    return dialogs[i].peer.id;
}

QVariant DialogItemModel::data(const QModelIndex &index, int role) const
{
    //TODO
    switch (role) {
    case Qt::DisplayRole: //title
    {
        return getDialogTitle(index.row());
    }
    case Qt::DecorationRole: //avatar
    {
        qint64 id = getDialogId(index.row());
        QPixmap avatar = avatars[id].value<QPixmap>();
        if (avatar.isNull()) avatar = thumbnails[id];
        return avatar;
    }
    case Qt::ToolTipRole: //last message
    {
        return getMessageString(index.row());
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

//TODO: FIXME: first and second request have duplicate results
void DialogItemModel::fetchMore(const QModelIndex &parent)
{
    if (!requestLock.tryLock()) return;
    if (requestId) return;

    requestId = client->getDialogs(offsetDate, offsetId, offsetPeer, 40);

    requestLock.unlock();
}

void DialogItemModel::client_gotDialogs(qint64 mtm, qint32 count, QList<TLDialog> d, QList<TLMessage> m, QList<TLChat> c, QList<TLUser> u)
{
    requestLock.lock();
    if (requestId != mtm) {
        requestLock.unlock();
        return;
    }

    requestId = 0;

    if (!count) gotFull = true;
    else gotFull |= (d.count() != 40);

    for (qint32 i = 0; i < m.size(); ++i) {
        TLMessage msg = m[i];
        messages.insert(msg.id, msg);
    }

    qint32 fH = QApplication::fontMetrics().height();
    fH += fH;
    fH += 12;

    for (qint32 i = 0; i < c.size(); ++i) {
        TLChat item = c[i];
        chats.insert(item.id, item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.title, fH));
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TLUser item = u[i];
        users.insert(item.id, item);
        //if (item.photo.type) avatars.insert(item.id, client->getFile(TLInputFileLocation(item.photo.photoSmall, TLInputPeer(item), false))); TODO
        thumbnails.insert(item.id, Avatars::generateThumbnail(item.id, item.firstName + " " + item.lastName, fH));
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

    beginInsertRows(QModelIndex(), dialogs.size(), dialogs.size() + d.size() - 1);

    dialogs.append(d);

    endInsertRows();

    requestLock.unlock();
}

void DialogItemModel::client_gotFilePart(qint64 mtMessageId, TLType::Types type, qint32 mtime, QByteArray bytes)
{
    qint64 id = avatars.key(mtMessageId);
    avatars.insert(id, QPixmap::fromImage(QImage::fromData(bytes)));
    thumbnails.remove(id);
}
