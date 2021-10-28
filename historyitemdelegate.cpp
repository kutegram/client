#include "historyitemdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QPixmap>
#include "library/tlmessages.h"
#include "avatars.h"
#include <Qt>

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
    TLMessage message = index.data().value<TLMessage>();

    QPixmap avatar = index.data(Qt::DecorationRole).value<QPixmap>();
    qint32 avatarSize = fm.height() * 2 + padding;

    qint32 maxWidth;
    qint32 initWidth = w - cloudPart - cloudPart - padding - padding - padding - avatarSize;

    QRect textRect = fm.boundingRect(x, y, initWidth, h, Qt::TextWordWrap, message.message);
    maxWidth = textRect.width();

    qint32 contentHeight = textRect.height();

    QFont nameFont(font);
    nameFont.setBold(true);
    QFontMetrics nameFM(nameFont);

    contentHeight += padding + nameFM.height();

    QVariant fromPeer = index.data(Qt::UserRole);

    QString peerName;
    TLUser user = qvariant_cast<TLUser>(fromPeer);
    if (user.type) peerName = user.firstName + " " + user.lastName;
    else {
        TLChat chat = qvariant_cast<TLChat>(fromPeer);
        peerName = chat.title;
    }

    QString elidedName = nameFM.elidedText(peerName, Qt::ElideRight, qMax(initWidth, maxWidth));
    maxWidth = qMax(maxWidth, nameFM.width(elidedName));

    QString replyText, replyPeerName;

    TLMessage replyMessage;

    if (message.reply.type) {
        replyMessage = index.data(Qt::ToolTipRole).value<TLMessage>();
        if (replyMessage.type) {
            replyText = fm.elidedText(replyMessage.message, Qt::ElideLeft, qMax(initWidth, maxWidth));
            maxWidth = qMax(maxWidth, fm.width(replyText) + padding + padding);

            QVariant replyPeer = index.data(Qt::ToolTipPropertyRole);
            TLUser replyUser = qvariant_cast<TLUser>(replyPeer);
            if (replyUser.type) replyPeerName = replyUser.firstName + " " + replyUser.lastName;
            else {
                TLChat replyChat = qvariant_cast<TLChat>(replyPeer);
                replyPeerName = replyChat.title;
            }

            replyPeerName = fm.elidedText(replyPeerName, Qt::ElideLeft, qMax(initWidth, maxWidth));
            maxWidth = qMax(maxWidth, fm.width(replyPeerName) + padding + padding);
        } else {
            replyText = replyPeerName = fm.elidedText(QApplication::translate("HistoryWindow", "Loading...", 0, QApplication::UnicodeUTF8), Qt::ElideLeft, qMax(initWidth, maxWidth));
            maxWidth = qMax(maxWidth, fm.width(replyText) + padding + padding);
        }

        contentHeight += padding + nameFM.height() + padding + fm.height();
    }

    qint32 cloudHeight = cloudPart + cloudPart + contentHeight;
    qint32 cloudWidth = cloudPart + cloudPart + maxWidth;

    qint32 avatarMargin = cloudHeight - avatarSize;

    if (!painter)
        return QSize(padding + avatarSize + padding + cloudWidth + padding, padding + cloudHeight + padding);

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
    painter->setPen(Avatars::userColor(message.from.id));
    painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextSingleLine, elidedName);

    y += padding + nameFM.height();

    if (message.reply.type) {
        QColor replyPeerColor = Avatars::userColor(replyMessage.from.id);
        painter->setBrush(replyPeerColor);
        painter->setPen(Qt::transparent);
        painter->drawRect(QRect(x, y, padding / 2, nameFM.height() + padding + fm.height()));

        x += padding / 2 + padding;

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
    painter->drawText(QRect(x, y, maxWidth, contentHeight), Qt::TextWordWrap, message.message);

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
