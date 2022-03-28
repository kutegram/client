#ifndef MESSAGEDOCUMENT_H
#define MESSAGEDOCUMENT_H

#include <QTextDocument>
#include "tl.h"

class MessageDocument : public QTextDocument
{
    Q_OBJECT
    Q_PROPERTY(TObject message READ message WRITE setMessage)
public:
    explicit MessageDocument(QObject *parent = 0);
    void setMessage(TObject message);
    TObject message() const;

    static QString parseHTML(TObject msg);
signals:
    
public slots:
    
private:
    TObject _message;
};

#endif // MESSAGEDOCUMENT_H
