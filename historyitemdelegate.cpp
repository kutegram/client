#include "historyitemdelegate.h"

HistoryItemDelegate::HistoryItemDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

void HistoryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TODO
}

QSize HistoryItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //TODO
    return option.rect.size();
}
