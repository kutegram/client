#include "messagelabel.h"

#include <QAbstractTextDocumentLayout>

MessageLabel::MessageLabel(TObject message, TObject from, TObject replyMessage, TObject replyPeer, QWidget *parent) :
    QTextBrowser(parent),
    message(message),
    from(from)
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

    QString html = peerNameToHtml(from);
    if (ID(message) != 0) html += replyToHtml(replyMessage, replyPeer);
    html += "<br>" + messageToHtml(message);

    setHtml(html);
}

void MessageLabel::adjustMinimumSize(const QSizeF& size)
{
    setMaximumHeight(size.height() + 2 * frameWidth());
    setMinimumHeight(size.height() + 2 * frameWidth());
}
