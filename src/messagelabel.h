#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QTextBrowser>
#include "messages.h"

class MessageLabel : public QTextBrowser
{
    Q_OBJECT
public:
    explicit MessageLabel(TObject m, QWidget *parent = 0);
    TObject message;
signals:
    
public slots:
    void adjustMinimumSize(const QSizeF& size);
};

#endif // MESSAGELABEL_H
