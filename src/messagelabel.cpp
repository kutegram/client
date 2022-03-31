#include "messagelabel.h"

#include <QAbstractTextDocumentLayout>

MessageLabel::MessageLabel(TObject message, TObject from, QWidget *parent) :
    QTextBrowser(parent),
    message(message),
    from(from)
{
    //setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

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
    setHtml(peerNameToHtml(from) + "<br>" + messageToHtml(message));
}

void MessageLabel::adjustMinimumSize(const QSizeF& size)
{
    setMinimumHeight(size.height() + 2 * frameWidth());
}
