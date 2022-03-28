#include "historyview.h"

#include "historyitemmodel.h"
#include <QScrollBar>

//TODO: canFetchUpwards on resize
//TODO: begin from first unread

HistoryView::HistoryView(QWidget *parent) :
    QListView(parent), lastMin(), lastMax()
{
    connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(verticalScrollbarRangeChanged(int,int)));

    setVerticalScrollMode(QListView::ScrollPerPixel);
    setHorizontalScrollMode(QListView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrap(true);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void HistoryView::verticalScrollbarValueChanged(int value)
{
    QListView::verticalScrollbarValueChanged(value);

    HistoryItemModel* _model = dynamic_cast<HistoryItemModel*>(model());
    if (!_model) return;
    if (verticalScrollBar()->minimum() == value && _model->canFetchMoreUpwards(rootIndex()))
        _model->fetchMoreUpwards(rootIndex());
}

void HistoryView::verticalScrollbarRangeChanged(int min, int max)
{
    qint32 lastVal = verticalScrollBar()->value();
    qint32 val = lastVal + max - min - lastMax + lastMin;
    if (!lastMin && !lastMax) verticalScrollBar()->setSliderPosition(max);
    else if (lastVal == lastMax || lastVal == lastMin) verticalScrollBar()->setSliderPosition(val);

    lastMin = min;
    lastMax = max;
}
