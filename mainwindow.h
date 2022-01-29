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
    void logoutAction_triggered();
    void quitAction_triggered();
    void aboutAction_triggered();
    void aboutQt_triggered();
    void dialogView_activated(QModelIndex index);
    void exitAction_triggered();
    void introButton_clicked();
    void codeButton_clicked();
    void loginButton_clicked();
    void changeLabel_linkActivated(QString link);

    void client_stateChanged(State state);
    void client_gotSentCode(qint64 mtm, QString phone_code_hash);

    void client_gotSocketError(QAbstractSocket::SocketError error);
    void client_gotMTError(qint32 error_code);
    void client_gotDHError(bool fail);
    void client_gotMessageError(qint64 mtm, qint32 error_code);
    void client_gotRPCError(qint64 mtm, qint32 error_code, QString error_message, bool handled);

private:
    FlickCharm flickcharm;
    TelegramClient *client;
    Ui::MainWindow *ui;
    QString phoneCodeHash;
    DialogItemModel *dialogModel;
    QAction exitAction;
#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    QAction optionsAction;
#endif

    void showTypeDialog();
};

#endif // MAINWINDOW_H
