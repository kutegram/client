#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QInputDialog>
#include "logintypedialog.h"
#include "qrlogindialog.h"
#include "dialogitemdelegate.h"
#include "dialogitemmodel.h"
#include "historywindow.h"
#include "main.h"
#include <QMessageBox>
#include <QtEndian>

#ifndef QT_NO_DEBUG_OUTPUT
#include <QtDebug>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    client(new TelegramClient),
    ui(new Ui::MainWindow),
    phoneNumber(),
    dialogModel(0),
    flickcharm(),
    backAction(this)
{
    ui->setupUi(this);

    connect(&backAction, SIGNAL(triggered()), this, SLOT(backAction_triggered()));
    backAction.setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        backAction.setIcon(QIcon(":/icons/back.svg"));
    }
    backAction.setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(&backAction);

    flickcharm.activateOn(ui->dialogView);

    ui->tabBar->setDrawBase(false);
    ui->dialogView->setModel(dialogModel = new DialogItemModel(client, ui->dialogView));
    ui->dialogView->setItemDelegate(new DialogItemDelegate(ui->dialogView));

    connect(client, SIGNAL(stateChanged(State)), this, SLOT(client_stateChanged(State)));
    connect(client, SIGNAL(gotLoginToken(qint64,qint32,QString)), this, SLOT(client_gotLoginToken(qint64,qint32,QString)));
    connect(client, SIGNAL(gotSentCode(qint64,QString)), this, SLOT(client_gotSentCode(qint64,QString)));

    connect(client, SIGNAL(gotSocketError(QAbstractSocket::SocketError)), this, SLOT(client_gotSocketError(QAbstractSocket::SocketError)));
    connect(client, SIGNAL(gotMTError(qint32)), this, SLOT(client_gotMTError(qint32)));
    connect(client, SIGNAL(gotDHError(bool)), this, SLOT(client_gotDHError(bool)));
    connect(client, SIGNAL(gotMessageError(qint64,qint32)), this, SLOT(client_gotMessageError(qint64,qint32)));
    connect(client, SIGNAL(gotRPCError(qint64,qint32,QString,bool)), this, SLOT(client_gotRPCError(qint64,qint32,QString,bool)));

    if (client->isLoggedIn()) client->start();
}

MainWindow::~MainWindow()
{
    client->stop();
    client->deleteLater();
    delete ui;
}

void MainWindow::showTypeDialog()
{
    LoginTypeDialog dialog(this);

    switch (dialog.exec()) {
    case 2:
    {
        //QR code
        client->exportLoginToken();
        break;
    }
    case 3:
    {
        //Phone number
        QInputDialog phoneInput(this);
        if (phoneInput.exec()) {
            phoneNumber = phoneInput.textValue();
            client->sendCode(phoneNumber);
        }

        break;
    }
    default:
    {
        break;
    }
    }
}

void MainWindow::loginAction_triggered()
{
    client->start();
}

void MainWindow::logoutAction_triggered()
{
    client->reset();
}

void MainWindow::exitAction_triggered()
{
    QApplication::quit();
}

void MainWindow::backAction_triggered()
{
    close();
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

void MainWindow::client_gotSocketError(QAbstractSocket::SocketError error)
{
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
    //setWindowTitle("Kutegram (ls: " + QString::number(state) + ")");
    switch (state) {
    case INITED:
    {
        if (!client->isLoggedIn()) showTypeDialog();
        break;
    }
    case LOGGED_IN:
    {
        if (dialogModel->canFetchMore(QModelIndex())) dialogModel->fetchMore(QModelIndex());
        break;
    }
    }
}

void MainWindow::client_gotLoginToken(qint64 mtm, qint32 expired, QString tokenUrl)
{
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug() << "Got qr token url:" << tokenUrl;
#endif

    QRLoginDialog dialog(tokenUrl, this);
    dialog.exec();
}

void MainWindow::client_gotSentCode(qint64 mtm, QString phone_code_hash)
{
    QInputDialog phoneInput(this);
    if (phoneInput.exec()) {
        client->signIn(phoneNumber, phone_code_hash, phoneInput.textValue());
    }
}

void MainWindow::dialogView_activated(QModelIndex index)
{
    HistoryWindow* window = new HistoryWindow(client, dialogModel->getInputPeer(index.row()), this);
    setOrientation(window, ScreenOrientationAuto);
    showExpanded(window);
}
