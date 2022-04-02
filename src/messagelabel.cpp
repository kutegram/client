#include "messagelabel.h"

#include <QAbstractTextDocumentLayout>

MessageLabel::MessageLabel(TObject message, TObject peer, TObject replyMessage, TObject replyPeer, QWidget *parent) :
    QTextBrowser(parent),
    message(),
    peer(),
    replyMessage(),
    replyPeer()
{
    setReadOnly(true);
    setFrameStyle(QFrame::NoFrame);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::transparent);
    setPalette(pal);

    setLineWrapMode(QTextEdit::WidgetWidth);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    connect(document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(adjustMinimumSize(QSizeF)));

    setTextInteractionFlags(Qt::TextBrowserInteraction);
    setOpenExternalLinks(false);
    setOpenLinks(false);

    setMessage(message, peer, replyMessage, replyPeer);
}

void MessageLabel::adjustMinimumSize(const QSizeF& size)
{
    setMaximumHeight(size.height() + 2 * frameWidth());
    setMinimumHeight(size.height() + 2 * frameWidth());
}

void MessageLabel::setMessage(TObject message, TObject peer, TObject replyMessage, TObject replyPeer)
{
    this->message = message;
    this->peer = peer;
    this->replyMessage = replyMessage;
    this->replyPeer = replyPeer;

    QString html = peerNameToHtml(peer);
    if (ID(message) != 0) html += replyToHtml(replyMessage, replyPeer);
    html += "<br>" + messageToHtml(message);

    setHtml(html);
}
