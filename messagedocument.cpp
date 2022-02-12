#include "messagedocument.h"

MessageDocument::MessageDocument(QObject *parent) :
    QTextDocument(parent),
    _message()
{

}

void MessageDocument::setMessage(TObject message)
{
    _message = message;
    setHtml(parseHTML(message));
}

TObject MessageDocument::message() const
{
    return _message;
}

QString MessageDocument::parseHTML(TObject msg)
{
    return QString(); //TODO
}
