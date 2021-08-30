#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "library/telegramclient.h"
#include "flickcharm.h"
#include <QModelIndex>

namespace Ui {
    class MainWindow;
}

class DialogItemModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

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
