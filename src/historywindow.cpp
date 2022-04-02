#include "historywindow.h"
#include "ui_historywindow.h"

#include "library/telegramclient.h"
#include <QScrollBar>
#include <QMutexLocker>
#include <QTextDocument>
#include <QTextBlock>
#include <QDomDocument>
#include <QDomNodeList>
#include <QXmlSimpleReader>
#include "messagelabel.h"
#include "tlschema.h"
#include "tl.h"
#include "devicehelper.h"

using namespace TLType;

HistoryWindow::HistoryWindow(TelegramClient *client, TObject input, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow),
    flickcharm(),
    client(client),
#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    optionsAction(this),
#endif
    backAction(this),
    peer(input),
    loadMutex(QMutex::Recursive),
    offsetId(),
    offsetDate(),
    requestId(),
    replyRequestId(),
    loadEnds(),
    messages(),
    labels(),
    replies(),
    chats(),
    users(),
    lastMin(),
    lastMax()
{
    ui->setupUi(this);
#if QT_VERSION >= 0x040702
    ui->messageEdit->setPlaceholderText(QApplication::translate("HistoryWindow", "Type a message...", 0, QApplication::UnicodeUTF8));
#endif

#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    /**
      * Qt installer for Symbian does not contain translation files (*.qm):
      * https://bugreports.qt.io/browse/QTBUG-4919
      * Create a custom "Options" softkey and localize it on the application level.
      */
    optionsAction.setText(QApplication::translate("HistoryWindow", "Options", 0, QApplication::UnicodeUTF8));
    QMenu* optionsMenu = new QMenu(this);
    // TODO: add actions for history window
    optionsAction.setMenu(optionsMenu);
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        optionsAction.setIcon(QIcon(":/icons/options.svg"));
    }
    optionsAction.setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(&optionsAction);
#endif

    connect(&backAction, SIGNAL(triggered()), this, SLOT(backAction_triggered()));
    backAction.setText(QApplication::translate("HistoryWindow", "Back", 0, QApplication::UnicodeUTF8));

#ifdef Q_OS_SYMBIAN
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        backAction.setIcon(QIcon(":/icons/back.svg"));
    }
#endif

    backAction.setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(&backAction);

    flickcharm.activateOn(ui->scrollArea);

    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
    connect(ui->scrollArea->verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(rangeChanged(int,int)));
    connect(client, SIGNAL(gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)), this, SLOT(client_gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)));
    connect(client, SIGNAL(updateNewMessage(TObject,qint32,qint32)), this, SLOT(client_updateNewMessage(TObject,qint32,qint32)));
    connect(client, SIGNAL(updateEditMessage(TObject,qint32,qint32)), this, SLOT(client_updateEditMessage(TObject,qint32,qint32)));
    connect(client, SIGNAL(updateDeleteMessages(TVector,qint32,qint32)), this, SLOT(client_updateDeleteMessages(TVector,qint32,qint32)));

    loadMessages();
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::sendButton_clicked()
{
    if (ui->messageEdit->text().isEmpty()) return;
    client->sendMessage(peer, ui->messageEdit->text());
    ui->messageEdit->clear();
}

void HistoryWindow::backAction_triggered()
{
    close();
}

void HistoryWindow::loadMessages()
{
    QMutexLocker locker(&loadMutex);

    if (replyRequestId || requestId || loadEnds) return;
    requestId = client->getHistory(peer, offsetId, offsetDate, 0, 40);
}

void HistoryWindow::client_gotMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    QMutexLocker locker(&loadMutex);

    if (mtm == requestId) {
        gotHistoryMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
        requestId = 0;
    }
    else if (mtm == replyRequestId) {
        gotReplyMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
        replyRequestId = 0;
    }
}

void HistoryWindow::gotHistoryMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    if (m.isEmpty()) loadEnds = true;

    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
    }

    TVector notFoundReplies;
    for (qint32 i = 0; i < m.size(); ++i) {
        TObject msg = m[i].toMap();
        messages.insert(msg["id"].toInt(), msg);

        if (!offsetDate || msg["date"].toInt() < offsetDate) {
            offsetDate = msg["date"].toInt();
            offsetId = msg["id"].toInt();
        }
    }

    for (qint32 i = 0; i < m.size(); ++i) {
        TObject msg = m[i].toMap();

        qint32 replyingId = msg["reply_to"].toMap()["reply_to_msg_id"].toInt();
        if (ID(msg["reply_to"].toMap()) && !ID(messages[replyingId])) {
            notFoundReplies.append(getInputMessage(msg["reply_to"].toMap()));
            replies.insert(replyingId, msg);
            TGOBJECT(emptyMessage, MessageEmpty);
            messages.insert(replyingId, emptyMessage);
        }

        addMessageWidget(msg, true);
    }

    if (!notFoundReplies.isEmpty())
        replyRequestId = client->getMessages(notFoundReplies);
}

void HistoryWindow::gotReplyMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
    }

    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
    }

    for (qint32 i = 0; i < m.size(); ++i) {
        TObject msg = m[i].toMap();
        messages.insert(msg["id"].toInt(), msg);
        updateMessageWidget(replies[msg["id"].toInt()]);
    }

    replies.clear();
}

TObject HistoryWindow::getMessagePeer(TObject msg)
{
    TObject from = msg["from_id"].toMap();
    switch (ID(from)) {
    case PeerChannel:
    case PeerChat:
        from = chats[getPeerId(from).toLongLong()];
        break;
    case PeerUser:
        from = users[getPeerId(from).toLongLong()];
        break;
    }

    if (ID(from) == 0) {
        from = msg["peer_id"].toMap();
        switch (ID(from)) {
        case PeerChannel:
        case PeerChat:
            from = chats[getPeerId(from).toLongLong()];
            break;
        case PeerUser:
            from = users[getPeerId(from).toLongLong()];
            break;
        }
    }

    return from;
}

void HistoryWindow::addMessageWidget(TObject msg, bool insert)
{
    TObject from = getMessagePeer(msg);
    TObject replyMessage;
    TObject replyPeer;
    if (ID(msg["reply_to"].toMap())) {
        replyMessage = messages[msg["reply_to"].toMap()["reply_to_msg_id"].toInt()];
        replyPeer = getMessagePeer(replyMessage);
    }

    MessageLabel* messageLabel = new MessageLabel(msg, from, replyMessage, replyPeer, ui->scrollArea_contents);
    connect(messageLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(messageAnchorClicked(QUrl)));
    labels.insert(msg["id"].toInt(), messageLabel);
    if (insert) ui->messagesLayout->insertWidget(1, messageLabel);
    else ui->messagesLayout->addWidget(messageLabel);
}

void HistoryWindow::updateMessageWidget(TObject msg)
{
    TObject from = getMessagePeer(msg);
    TObject replyMessage;
    TObject replyPeer;
    if (ID(msg["reply_to"].toMap())) {
        replyMessage = messages[msg["reply_to"].toMap()["reply_to_msg_id"].toInt()];
        replyPeer = getMessagePeer(replyMessage);
    }

    MessageLabel* messageLabel = labels[msg["id"].toInt()];
    if (!messageLabel) return;
    messageLabel->setMessage(msg, from, replyMessage, replyPeer);
}

void HistoryWindow::client_updateNewMessage(TObject msg, qint32 pts, qint32 pts_count)
{
    QMutexLocker locker(&loadMutex);

    //TODO: test PTS sequence
    //TODO: load needed information
    if (getPeerId(msg["peer_id"].toMap()) != getPeerId(peer)) return;

    messages.insert(msg["id"].toInt(), msg);

    addMessageWidget(msg, false);
}

void HistoryWindow::client_updateEditMessage(TObject msg, qint32 pts, qint32 pts_count)
{
    QMutexLocker locker(&loadMutex);

    //TODO: test PTS sequence
    //TODO: load needed information
    if (getPeerId(msg["peer_id"].toMap()) != getPeerId(peer)) return;

    messages.insert(msg["id"].toInt(), msg);

    updateMessageWidget(msg);
}

void HistoryWindow::client_updateDeleteMessages(TVector msgs, qint32 pts, qint32 pts_count)
{
    QMutexLocker locker(&loadMutex);

    //TODO: test PTS sequence
    //TODO: load needed information

    for (qint32 i = 0; i < msgs.size(); ++i) {
        qint32 id = msgs[i].toInt();
        messages.remove(id);
        MessageLabel* messageLabel = labels[id];
        labels.remove(id);
        if (!messageLabel) continue;
        ui->messagesLayout->removeWidget(messageLabel);
        messageLabel->deleteLater();
    }
}

void HistoryWindow::messageAnchorClicked(const QUrl &link)
{
    if (link.scheme() == "kutegram") {
        if (link.host() == "spoiler") {
            MessageLabel* s = static_cast<MessageLabel*>(sender());
            if (!s) return;

            QDomDocument dom;
            dom.setContent(s->document()->toHtml(), false);
            QDomNodeList list = dom.elementsByTagName("a");
            for (qint32 i = 0; i < list.count(); ++i) {
                QDomElement node = list.at(i).toElement();
                if (node.attribute("href") == link.toString()) {
                    node.removeAttribute("href");
                    node.firstChildElement("span").setAttribute("style", "background-color:lightgray;");
                    break;
                }
            }
            s->setHtml(dom.toString(-1));
        }
    }
    else openUrl(link);
}

void HistoryWindow::sliderReleased()
{
    if (ui->scrollArea->verticalScrollBar()->value() == ui->scrollArea->verticalScrollBar()->minimum()) {
        loadMessages();
    }
}

void HistoryWindow::rangeChanged(int min, int max)
{
    if (ui->scrollArea->verticalScrollBar()->value() == lastMax) {
        ui->scrollArea->verticalScrollBar()->setValue(max);
    } else {
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() - (lastMax - lastMin) + (max - min));
    }

    lastMax = max;
    lastMin = min;
}

void HistoryWindow::valueChanged(int value)
{
    //qDebug() << value;
}
