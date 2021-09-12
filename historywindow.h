#ifndef HISTORYWINDOW_H
#define HISTORYWINDOW_H

#include <QMainWindow>
#include "library/tlmessages.h"
#include "flickcharm.h"

namespace Ui {
class HistoryWindow;
}

class TelegramClient;

class HistoryWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit HistoryWindow(TelegramClient *client, TLInputPeer input, QWidget *parent = 0);
    ~HistoryWindow();
    
private:
    Ui::HistoryWindow *ui;
    FlickCharm flickcharm;
};

#endif // HISTORYWINDOW_H
