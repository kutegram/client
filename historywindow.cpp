#include "historywindow.h"
#include "ui_historywindow.h"

#include "historyitemmodel.h"
#include "historyitemdelegate.h"

HistoryWindow::HistoryWindow(TelegramClient *client, TLInputPeer input, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow),
    flickcharm()
{
    ui->setupUi(this);

    flickcharm.activateOn(ui->historyView);

    ui->historyView->setModel(new HistoryItemModel(client, input, ui->historyView));
    ui->historyView->setItemDelegate(new HistoryItemDelegate(ui->historyView));
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}
