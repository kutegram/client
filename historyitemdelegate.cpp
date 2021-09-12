#include "historyitemdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QPixmap>

HistoryItemDelegate::HistoryItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

void HistoryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    qint32 cloudPart = option.fontMetrics.height() / 2;

    x += padding;
    y += padding;

    QString text = index.data().toString();

    QRect textRect = option.fontMetrics.boundingRect(QRect(option.rect.x(), option.rect.y(), option.rect.width() - padding - cloudPart - padding - cloudPart, option.rect.height()), Qt::TextWordWrap, text);
    painter->setBrush(QColor(240, 240, 240));
    painter->setPen(Qt::transparent);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundedRect(QRect(x, y, cloudPart + cloudPart + textRect.width(), cloudPart + cloudPart + textRect.height()), padding, padding);

    x += cloudPart;
    y += cloudPart;

    //painter->setPen((option.state & QStyle::State_HasFocus) == QStyle::State_HasFocus ? option.palette.brightText().color() : option.palette.text().color());
    painter->setPen(option.palette.text().color());
    painter->drawText(QRect(x, y, option.rect.width() - padding - cloudPart - padding - cloudPart, option.rect.height()), Qt::TextWordWrap, text);
}

QSize HistoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TODO
    qint32 padding = 8;
    qint32 cloudPart = option.fontMetrics.height() & ~1;
    qint32 stringHeight = option.fontMetrics.boundingRect(QRect(option.rect.x(), option.rect.y(), option.rect.width() - padding - cloudPart, option.rect.height()), Qt::TextWordWrap, index.data().toString()).height();

    return QSize(option.rect.width(), padding + cloudPart + stringHeight);
}
