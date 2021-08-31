#ifndef HISTORYITEMDELEGATE_H
#define HISTORYITEMDELEGATE_H

#include <QAbstractItemDelegate>

class HistoryItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit HistoryItemDelegate(QObject *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
    
public slots:
    
};

#endif // HISTORYITEMDELEGATE_H
