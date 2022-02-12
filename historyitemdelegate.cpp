#include "historyitemdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QPixmap>
#include "tl.h"
#include "avatars.h"
#include <Qt>
#include "tlschema.h"
#include <QDebug>

using namespace TLType;

HistoryItemDelegate::HistoryItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

QSize paintContent(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    qint32 x = option.rect.x();
    qint32 y = option.rect.y();
    qint32 w = option.rect.width();
    qint32 h = option.rect.height();

    QFontMetrics fm = QApplication::fontMetrics();
    QFont font = QApplication::font();

    qint32 padding = 4;
    qint32 cloudPart = fm.height() / 3;
    qint32 spacePadding = fm.width(' ');
    TObject message = index.data().toMap();

    QPixmap avatar = index.data(Qt::DecorationRole).value<QPixmap>();
    qint32 avatarSize = fm.height(); avatarSize += avatarSize;

    qint32 maxWidth;
    qint32 initWidth = w - cloudPart - cloudPart - padding - padding - padding - avatarSize;

    QRect textRect = fm.boundingRect(x, y, initWidth, h, Qt::TextWordWrap, message["message"].toString());
    maxWidth = textRect.width();

    qint32 contentHeight = textRect.height();

    QFont nameFont(font);
    nameFont.setBold(true);
    QFontMetrics nameFM(nameFont);

    contentHeight += padding + nameFM.height();

    TObject fromPeer = index.data(Qt::UserRole).toMap();

    QString peerName;

    switch (ID(fromPeer)) {
    case UserEmpty:
    case User:
        peerName = fromPeer["first_name"].toString() + " " + fromPeer["last_name"].toString();
        break;
    default:
        //this is a chat, probably.
        peerName = fromPeer["title"].toString();
        break;
    }

    QString elidedName = nameFM.elidedText(peerName, Qt::ElideRight, qMax(initWidth, maxWidth));
    maxWidth = qMax(maxWidth, nameFM.width(elidedName));

    QString replyText, replyPeerName;

    TObject replyMessage;

    if (ID(message["reply_to"].toMap())) {
        replyMessage = index.data(Qt::ToolTipRole).toMap();
        if (ID(replyMessage)) {
            replyText = fm.elidedText(replyMessage["message"].toString(), Qt::ElideLeft, qMax(initWidth, maxWidth) - spacePadding - padding);
            maxWidth = qMax(maxWidth, fm.width(replyText) + spacePadding + padding);

            TObject replyPeer = index.data(Qt::ToolTipPropertyRole).toMap();

            QString replyPeerName;

            switch (ID(replyPeer)) {
            case UserEmpty:
            case User:
                replyPeerName = replyPeer["first_name"].toString() + " " + replyPeer["last_name"].toString();
                break;
            default:
                //this is a chat, probably.
                replyPeerName = replyPeer["title"].toString();
                break;
            }

            replyPeerName = fm.elidedText(replyPeerName, Qt::ElideLeft, qMax(initWidth, maxWidth) - spacePadding - padding);
            maxWidth = qMax(maxWidth, fm.width(replyPeerName) + spacePadding + padding);
        } else {
            replyText = replyPeerName = fm.elidedText(QApplication::translate("HistoryWindow", "Loading...", 0, QApplication::UnicodeUTF8), Qt::ElideLeft, qMax(initWidth, maxWidth) - spacePadding - padding);
            maxWidth = qMax(maxWidth, fm.width(replyText) + spacePadding + padding);
        }

        contentHeight += padding + nameFM.height() + padding + fm.height();
    }

    qint32 cloudHeight = cloudPart + cloudPart + contentHeight;
    qint32 cloudWidth = cloudPart + cloudPart + maxWidth;

    qint32 avatarMargin = cloudHeight - avatarSize;

    if (!painter)
        //return QSize(padding + avatarSize + padding + cloudWidth + padding, padding + cloudHeight + padding);
        return QSize(option.rect.width(), padding + cloudHeight + padding);

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    x += padding;
    y += padding;

    y += avatarMargin;

    painter->drawPixmap(x, y, avatarSize, avatarSize, avatar);

    y -= avatarMargin;

    x += avatarSize + padding;

    painter->setBrush(QColor(240, 240, 240));
    painter->setPen(Qt::transparent);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(QRect(x, y, cloudWidth, cloudHeight), cloudPart, cloudPart);
    painter->setRenderHint(QPainter::Antialiasing, false);

    x += cloudPart;
    y += cloudPart;

    painter->setFont(nameFont);
    painter->setPen(Avatars::userColor(getPeerId(message["from_id"].toMap()).toLongLong()));
    painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextSingleLine, elidedName);

    y += padding + nameFM.height();

    if (ID(message["reply_to"].toMap())) {
        QColor replyPeerColor = Avatars::userColor(getPeerId(replyMessage["from_id"].toMap()).toLongLong());
        painter->setBrush(replyPeerColor);
        painter->setPen(Qt::transparent);
        painter->drawRect(QRect(x, y, spacePadding, nameFM.height() + padding + fm.height()));

        x += spacePadding + padding;

        painter->setFont(nameFont);
        painter->setPen(replyPeerColor);
        painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextSingleLine, replyPeerName);

        y += padding + nameFM.height();

        painter->setFont(font);
        painter->setPen(Qt::black);
        painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextSingleLine, replyText);

        y += padding + fm.height();

        x -= padding + padding;
    }

    painter->setFont(font);
    painter->setPen(Qt::black);
    painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextWordWrap, message["message"].toString());

    return QSize();
}

void HistoryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    paintContent(painter, option, index);
}

QSize HistoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return paintContent(0, option, index);
}
