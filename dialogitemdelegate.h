#ifndef DIALOGITEMDELEGATE_H
#define DIALOGITEMDELEGATE_H

#include <QAbstractItemDelegate>

class DialogItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit DialogItemDelegate(QObject *parent = 0);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
signals:
    
public slots:
    
};

#endif // DIALOGITEMDELEGATE_H
