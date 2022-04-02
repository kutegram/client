#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QInputDialog>
#include "dialogitemdelegate.h"
#include "dialogitemmodel.h"
#include "historywindow.h"
#include "main.h"
#include <QMessageBox>
#include <QtEndian>
#include <QDebug>
#include "devicehelper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    client(new TelegramClient),
    ui(new Ui::MainWindow),
    phoneCodeHash(),
    dialogModel(0),
    flickcharm(),
#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    optionsAction(this),
#endif
    exitAction(this)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(client->isLoggedIn());
    ui->introStackedWidget->setCurrentIndex(0);

#if QT_VERSION >= 0x040702
    ui->phoneEdit->setPlaceholderText(QApplication::translate("MainWindow", "Phone number", 0, QApplication::UnicodeUTF8));
    ui->codeEdit->setPlaceholderText(QApplication::translate("MainWindow", "Code", 0, QApplication::UnicodeUTF8));
#endif

#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    /**
      * Qt installer for Symbian does not contain translation files (*.qm):
      * https://bugreports.qt.io/browse/QTBUG-4919
      * Create a custom "Options" softkey and localize it on the application level.
      */
    optionsAction.setText(QApplication::translate("MainWindow", "Options", 0, QApplication::UnicodeUTF8));
    QMenu* optionsMenu = new QMenu(this);
    optionsMenu->addAction(ui->actionsMenu->menuAction());
    optionsMenu->addAction(ui->helpMenu->menuAction());
    optionsAction.setMenu(optionsMenu);
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        optionsAction.setIcon(QIcon(":/icons/options.svg"));
    }
    optionsAction.setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(&optionsAction);
#endif

    connect(&exitAction, SIGNAL(triggered()), this, SLOT(exitAction_triggered()));
    exitAction.setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));

#ifdef Q_OS_SYMBIAN
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        exitAction.setIcon(QIcon(":/icons/back.svg"));
    }
#endif

    exitAction.setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(&exitAction);

    flickcharm.activateOn(ui->dialogView);

    ui->tabBar->setDrawBase(false);
    ui->tabBar->addTab(QApplication::translate("MainWindow", "All chats", 0, QApplication::UnicodeUTF8));

    ui->dialogView->setModel(dialogModel = new DialogItemModel(client, ui->dialogView));
    ui->dialogView->setItemDelegate(new DialogItemDelegate(ui->dialogView));

    connect(client, SIGNAL(stateChanged(State)), this, SLOT(client_stateChanged(State)));
    connect(client, SIGNAL(gotSentCode(qint64,QString)), this, SLOT(client_gotSentCode(qint64,QString)));

    connect(client, SIGNAL(gotSocketError(QAbstractSocket::SocketError)), this, SLOT(client_gotSocketError(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(gotMTError(qint32)), this, SLOT(client_gotMTError(qint32)));
    connect(client, SIGNAL(gotDHError(bool)), this, SLOT(client_gotDHError(bool)));
    connect(client, SIGNAL(gotMessageError(qint64,qint32)), this, SLOT(client_gotMessageError(qint64,qint32)));
    connect(client, SIGNAL(gotRPCError(qint64,qint32,QString,bool)), this, SLOT(client_gotRPCError(qint64,qint32,QString,bool)));

    connect(client, SIGNAL(updateNewMessage(TObject,qint32,qint32)), this, SLOT(client_updateNewMessage(TObject,qint32,qint32)));

    if (client->isLoggedIn()) client->start();
}

MainWindow::~MainWindow()
{
    client->stop();
    client->deleteLater();
    delete ui;
}

void MainWindow::logoutAction_triggered()
{
    client->reset();
    ui->introStackedWidget->slideInIdx(3);
    ui->stackedWidget->slideInIdx(0);
}

void MainWindow::quitAction_triggered()
{
    QApplication::quit();
}

void MainWindow::exitAction_triggered()
{
    close();
    //_exit(0); //recommendation for Harmattan
}

void MainWindow::aboutAction_triggered()
{
    QMessageBox::about(
                this,
                QApplication::translate("MainWindow", "Kutegram", 0, QApplication::UnicodeUTF8),
                QApplication::translate("MainWindow", "Kutegram by curoviyxru\nAn unofficial Qt-based client for Telegram messenger.\nProject's website: http://kg.curoviyx.ru\nTelegram channel: https://t.me/kutegram\nTelegram chat: https://t.me/kutegramchat", 0, QApplication::UnicodeUTF8)
                       );
}

void MainWindow::aboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::introButton_clicked()
{
    client->start();
}

void MainWindow::codeButton_clicked()
{
    client->sendCode(ui->phoneEdit->text());
}

void MainWindow::loginButton_clicked()
{
    client->signIn(ui->phoneEdit->text(), phoneCodeHash, ui->codeEdit->text());
}

void MainWindow::changeLabel_linkActivated(QString link)
{
    ui->stackedWidget->slideInIdx(0);
    ui->introStackedWidget->slideInIdx(1);
}

void MainWindow::client_gotSocketError(QAbstractSocket::SocketError error)
{
    //TODO: move this to title
    QMessageBox::critical(
                this,
                QApplication::translate("MainWindow", "Error", 0, QApplication::UnicodeUTF8),
                QApplication::translate("MainWindow", "Got socket error: %1", 0, QApplication::UnicodeUTF8)
                .arg(QString::number(error))
                );
}

void MainWindow::client_gotMTError(qint32 error_code)
{
    QMessageBox::critical(
                this,
                QApplication::translate("MainWindow", "Error", 0, QApplication::UnicodeUTF8),
                QApplication::translate("MainWindow", "Got MT error: %1", 0, QApplication::UnicodeUTF8)
                .arg(QString::number(error_code))
                );
}

void MainWindow::client_gotDHError(bool fail)
{
    QMessageBox::critical(
                this,
                QApplication::translate("MainWindow", "Error", 0, QApplication::UnicodeUTF8),
                QApplication::translate("MainWindow", "Got DH error: %1", 0, QApplication::UnicodeUTF8)
                .arg(QString::number(fail))
                );
}

void MainWindow::client_gotMessageError(qint64 mtm, qint32 error_code)
{
    //TODO: do we need message error?
}

void MainWindow::client_gotRPCError(qint64 mtm, qint32 error_code, QString error_message, bool handled)
{
    if (handled) return;
    QMessageBox::critical(
                this,
                QApplication::translate("MainWindow", "Error", 0, QApplication::UnicodeUTF8),
                QApplication::translate("MainWindow", "Got RPC error: %1, %2, %3", 0, QApplication::UnicodeUTF8)
                .arg(QString::number(client->messageConstructor(mtm))).arg(QString::number(error_code)).arg(error_message)
                );
}

void MainWindow::client_stateChanged(State state)
{
    qDebug() << "[D] New state:" << state;
    switch (state) {
    case INITED:
        if (!client->isLoggedIn()) {
            ui->stackedWidget->slideInIdx(0);
            ui->introStackedWidget->slideInIdx(1);
        }
        break;
    case LOGGED_IN:
        ui->stackedWidget->slideInIdx(1);
        if (dialogModel->canFetchMore(QModelIndex())) dialogModel->fetchMore(QModelIndex());
        break;
    }
}

void MainWindow::client_gotSentCode(qint64 mtm, QString phone_code_hash)
{
    if (!client->isLoggedIn()) {
        phoneCodeHash = phone_code_hash;
        ui->stackedWidget->slideInIdx(0);
        ui->introStackedWidget->slideInIdx(2);
    }
}

void MainWindow::dialogView_activated(QModelIndex index)
{
    HistoryWindow* window = new HistoryWindow(client, dialogModel->getInputPeerByIndex(index.row()), this);
    setOrientation(window, ScreenOrientationAuto);
    showExpanded(window);
}

void MainWindow::client_updateNewMessage(TObject msg, qint32 pts, qint32 pts_count)
{
    //TODO: improve it.
    if (getPeerId(msg["from_id"].toMap()).toLongLong() == client->userId() ||
        getPeerId(msg["peer_id"].toMap()).toLongLong() == client->userId()) return;

    showNotification(QApplication::translate("MainWindow", "New message", 0, QApplication::UnicodeUTF8),
            msg["message"].toString().mid(0, 40));
}
