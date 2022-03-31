#include "historywindow.h"
#include "ui_historywindow.h"

#include "library/telegramclient.h"
#include <QScrollBar>
#include <QMutexLocker>
#include "messagelabel.h"
#include "tlschema.h"
#include "tl.h"
#include "main.h"

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
    messages(),
    chats(),
    users()
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

    connect(client, SIGNAL(gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)), this, SLOT(client_gotMessages(qint64,qint32,TVector,TVector,TVector,qint32,qint32,bool)));
    connect(client, SIGNAL(updateNewMessage(TObject,qint32,qint32)), this, SLOT(client_updateNewMessage(TObject,qint32,qint32)));

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

    if (requestId) return;
    requestId = client->getHistory(peer, offsetId, offsetDate, 0, 40);
}

void HistoryWindow::client_gotMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    QMutexLocker locker(&loadMutex);

    if (mtm == requestId) {
        gotHistoryMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
    }
//    else if (mtm == replyRequestId) {
//        gotReplyMessages(mtm, count, m, c, u, offsetIdOffset, nextRate, inexact);
//    }
}

void HistoryWindow::gotHistoryMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact)
{
    for (qint32 i = 0; i < c.size(); ++i) {
        TObject item = c[i].toMap();
        chats.insert(item["id"].toLongLong(), item);
    }
    for (qint32 i = 0; i < u.size(); ++i) {
        TObject item = u[i].toMap();
        users.insert(item["id"].toLongLong(), item);
    }

    for (qint32 i = m.size() - 1; i >= 0; --i) {
        TObject msg = m[i].toMap();
        messages.insert(msg["id"].toInt(), msg);

        if (!offsetDate || msg["date"].toInt() < offsetDate) {
            offsetDate = msg["date"].toInt();
            offsetId = msg["id"].toInt();
        }

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

        MessageLabel* messageLabel = new MessageLabel(msg, from, ui->scrollArea_contents);
        connect(messageLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(messageAnchorClicked(QUrl)));
        ui->messagesLayout->addWidget(messageLabel);
    }
}

void HistoryWindow::client_updateNewMessage(TObject message, qint32 pts, qint32 pts_count)
{
//    QMutexLocker locker(&requestLock);

//    //TODO: test PTS sequence
//    //TODO: load needed information
//    if (getPeerId(message["peer_id"].toMap()) != getPeerId(peer)) return;

//    beginInsertRows(QModelIndex(), history.size(), history.size());

//    history.append(message["id"].toInt());
//    messages.insert(message["id"].toInt(), message);

//    endInsertRows();
}

void HistoryWindow::messageAnchorClicked(const QUrl &link)
{
    if (link.scheme() == "kutegram") return;
    openUrl(link);
}
