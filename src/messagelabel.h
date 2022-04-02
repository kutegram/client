#ifndef MESSAGELABEL_H
#define MESSAGELABEL_H

#include <QTextBrowser>
#include "messages.h"

class MessageLabel : public QTextBrowser
{
    Q_OBJECT
public:
    explicit MessageLabel(TObject message, TObject peer, TObject replyMessage, TObject replyPeer, QWidget *parent = 0);
    TObject message;
    TObject peer;
    TObject replyMessage;
    TObject replyPeer;
    void setMessage(TObject message, TObject peer, TObject replyMessage, TObject replyPeer);
signals:
    
public slots:
    void adjustMinimumSize(const QSizeF& size);
};

#endif // MESSAGELABEL_H
