#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QModelIndex>
#include "flickcharm.h"
#include "library/telegramclient.h"
#include <QAction>

namespace Ui {
    class MainWindow;
}

class DialogItemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

public slots:
    void loginAction_triggered();
    void logoutAction_triggered();
    void exitAction_triggered();
    void aboutAction_triggered();
    void aboutQt_triggered();
    void dialogView_activated(QModelIndex index);

    void client_stateChanged(State state);
    void client_gotLoginToken(qint64 mtm, qint32 expired, QString tokenUrl);
    void client_gotSentCode(qint64 mtm, QString phone_code_hash);

    void client_gotSocketError(QAbstractSocket::SocketError error);
    void client_gotMTError(qint32 error_code);
    void client_gotDHError(bool fail);
    void client_gotMessageError(qint64 mtm, qint32 error_code);
    void client_gotRPCError(qint64 mtm, qint32 error_code, QString error_message);

    void backAction_triggered();

private:
    FlickCharm flickcharm;
    TelegramClient *client;
    Ui::MainWindow *ui;
    QString phoneNumber;
    DialogItemModel *dialogModel;
    QAction backAction;

    void showTypeDialog();
};

#endif // MAINWINDOW_H
