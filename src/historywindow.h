#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QMainWindow>
#include "tl.h"
#include "flickcharm.h"
#include <QAction>
#include <QMutex>
#include <QHash>

namespace Ui {
class HistoryWindow;
}

class TelegramClient;

class HistoryWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit HistoryWindow(TelegramClient *client, TObject input, QWidget *parent = 0);
    ~HistoryWindow();

    void loadMessages();
    void gotHistoryMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    void gotReplyMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    void addMessageWidget(TObject msg, bool insert);
    TObject getMessagePeer(TObject msg);

public slots:
    void sendButton_clicked();
    void backAction_triggered();
    void client_gotMessages(qint64 mtm, qint32 count, TVector m, TVector c, TVector u, qint32 offsetIdOffset, qint32 nextRate, bool inexact);
    void client_updateNewMessage(TObject message, qint32 pts, qint32 pts_count);
    void messageAnchorClicked(const QUrl &link);
    void sliderReleased();
    void valueChanged(int value);
    void rangeChanged(int min, int max);

private:
    Ui::HistoryWindow *ui;
    FlickCharm flickcharm;
    TelegramClient* client;
    QAction backAction;
#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    QAction optionsAction;
#endif

    TObject peer;
    QMutex loadMutex;
    qint32 offsetId;
    qint32 offsetDate;
    qint64 requestId;
    bool loadEnds;
    QHash<qint32, TObject> messages;
    QHash<qint64, TObject> chats;
    QHash<qint64, TObject> users;

    qint32 lastMin;
    qint32 lastMax;
};

#endif // HISTORYWINDOW_H
