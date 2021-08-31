#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QModelIndex>
#include "flickcharm.h"
#include "library/telegramclient.h"

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
    void dialogView_activated(QModelIndex index);

    void client_stateChanged(State state);
    void client_gotLoginToken(qint32 expired, QString tokenUrl);
    void client_gotSentCode(QString phone_code_hash);

private:
    FlickCharm flickcharm;
    TelegramClient *client;
    Ui::MainWindow *ui;
    QString phoneNumber;
    DialogItemModel *dialogModel;

    void showTypeDialog();
};

#endif // MAINWINDOW_H
