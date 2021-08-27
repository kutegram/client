#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QInputDialog>
#include "logintypedialog.h"
#include "qrlogindialog.h"
#include "dialogitemdelegate.h"
#include "dialogitemmodel.h"

#ifndef QT_NO_DEBUG_OUTPUT
#include <QtDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), client(new TelegramClient), ui(new Ui::MainWindow), phoneNumber(), dialogModel(0)
{
    ui->setupUi(this);
    ui->tabBar->setDrawBase(false);
    ui->dialogView->setModel(dialogModel = new DialogItemModel(client, ui->dialogView));
    ui->dialogView->setItemDelegate(new DialogItemDelegate(ui->dialogView));

    connect(client, SIGNAL(stateChanged(State)), this, SLOT(client_stateChanged(State)));
    connect(client, SIGNAL(gotLoginToken(qint32,QString)), this, SLOT(client_gotLoginToken(qint32,QString)));
    connect(client, SIGNAL(gotSentCode(QString)), this, SLOT(client_gotSentCode(QString)));
}

MainWindow::~MainWindow()
{
    client->deleteLater();
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    //showFullScreen();
    showMaximized();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
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
    if (client->isConnected()) showTypeDialog();
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
        ui->statusBar->showMessage("Got state: " + QString::number(state), 5000);
        break;
    }
    }
}

void MainWindow::client_gotLoginToken(qint32 expired, QString tokenUrl)
{
#ifndef QT_NO_DEBUG_OUTPUT
    qDebug() << "Got qr token url:" << tokenUrl;
#endif

    QRLoginDialog dialog(tokenUrl, this);
    dialog.exec();
}

void MainWindow::client_gotSentCode(QString phone_code_hash)
{
    QInputDialog phoneInput(this);
    if (phoneInput.exec()) {
        client->signIn(phoneNumber, phone_code_hash, phoneInput.textValue());
    }
}
