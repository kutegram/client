#ifndef HISTORYVIEW_H
#define HISTORYVIEW_H

#include <QListView>

class HistoryView : public QListView
{
    Q_OBJECT
private:
    qint32 lastMin, lastMax;
public:
    explicit HistoryView(QWidget *parent = 0);
    
signals:
    
public slots:
    void verticalScrollbarValueChanged(int value);
    void verticalScrollbarRangeChanged(int min, int max);
};

#endif // HISTORYVIEW_H
