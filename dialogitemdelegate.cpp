#include "dialogitemdelegate.h"

#include <QPainter>

DialogItemDelegate::DialogItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

void DialogItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) return;

    qint32 x = option.rect.x();
    qint32 y = option.rect.y();
    qint32 w = option.rect.width();
    qint32 h = option.rect.height();
    QFontMetrics fm = option.fontMetrics;

    qint32 padding = fm.lineSpacing();
    qint32 avatarSize = fm.height() + padding + fm.height();

    painter->setBrush(Qt::black);
    painter->setFont(option.font);
    painter->drawText(x, y, w, h, Qt::TextSingleLine, fm.elidedText(index.data().toString(), Qt::ElideRight, w));
}

QSize DialogItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) return option.rect.size();
    return QSize(option.rect.width(), option.fontMetrics.height() + option.fontMetrics.lineSpacing() + option.fontMetrics.height());
}
