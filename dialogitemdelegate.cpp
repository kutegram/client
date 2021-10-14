#include "dialogitemdelegate.h"

#include "avatars.h"
#include <QPainter>
#include <QApplication>
#include <QPixmap>

DialogItemDelegate::DialogItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

void DialogItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TODO
    if (!index.isValid()) return;

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter);

    qint32 x = option.rect.x();
    qint32 y = option.rect.y();
    qint32 w = option.rect.width();
    qint32 h = option.rect.height();

    QFontMetrics fm = option.fontMetrics;
    QFont font = option.font;

    qint32 padding = 4;
    qint32 fontHeight = fm.height();
    qint32 avatarSize = padding + fontHeight + padding + fontHeight + padding;
    qint32 height = padding + avatarSize + padding;

    QString title = index.data().toString();

    x += padding;
    y += padding;

    QPixmap avatar = index.data(Qt::DecorationRole).value<QPixmap>();
    QRect circleRect(x, y, avatarSize, avatarSize);
    //TODO: Cache it.
    if (avatar.isNull()) avatar = Avatars::generateThumbnail(index.data(Qt::UserRole).toLongLong(), title, avatarSize);
    painter->drawPixmap(circleRect, avatar);

    x += avatarSize + padding + padding;
    y += padding;

    painter->setPen((option.state & QStyle::State_HasFocus) == QStyle::State_HasFocus ? option.palette.brightText().color() : option.palette.text().color());

    font.setBold(true);
    painter->setFont(font);
    painter->drawText(x, y, w, h, Qt::TextSingleLine, fm.elidedText(title, Qt::ElideRight, w - x - padding));

    y += fontHeight + padding;

    QString tooltip = index.data(Qt::ToolTipRole).toString();

    font.setBold(false);
    painter->setFont(font);
    painter->drawText(x, y, w, h, Qt::TextSingleLine, fm.elidedText(tooltip, Qt::ElideRight, w - x - padding));
}

QSize DialogItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) return option.rect.size();

    QFontMetrics fm = option.fontMetrics;

    qint32 padding = 4;
    qint32 fontHeight = fm.height();

    return QSize(option.rect.width(), padding + padding + fontHeight + padding + fontHeight + padding + padding);
}
