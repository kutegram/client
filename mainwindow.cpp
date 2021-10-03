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

#ifndef QT_NO_DEBUG_OUTPUT
#include <QtDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), client(new TelegramClient), ui(new Ui::MainWindow), phoneNumber(), dialogModel(0), flickcharm()
{
    ui->setupUi(this);

    flickcharm.activateOn(ui->dialogView);

    ui->tabBar->setDrawBase(false);
    ui->dialogView->setModel(dialogModel = new DialogItemModel(client, ui->dialogView));
    ui->dialogView->setItemDelegate(new DialogItemDelegate(ui->dialogView));

    connect(client, SIGNAL(stateChanged(State)), this, SLOT(client_stateChanged(State)));
    connect(client, SIGNAL(gotLoginToken(qint64,qint32,QString)), this, SLOT(client_gotLoginToken(qint64,qint32,QString)));
    connect(client, SIGNAL(gotSentCode(qint64,QString)), this, SLOT(client_gotSentCode(qint64,QString)));
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

void MainWindow::client_stateChanged(State state)
{
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
    default:
    {
        //TODO
        setWindowTitle("Kutegram (ls: " + QString::number(state) + ")");
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
    if (!index.isValid()) return;

    HistoryWindow* window = new HistoryWindow(client, dialogModel->getInputPeer(index.row()), this);
    setOrientation(window, ScreenOrientationAuto);
    showExpanded(window);
}
