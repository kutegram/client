#include "historywindow.h"
#include "ui_historywindow.h"

#include "historyitemmodel.h"
#include "historyitemdelegate.h"

HistoryWindow::HistoryWindow(TelegramClient *client, TLInputPeer input, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow)
{
    ui->setupUi(this);
    ui->historyView->setModel(new HistoryItemModel(client, input, ui->historyView));
    //ui->historyView->setItemDelegate(new HistoryItemDelegate(ui->historyView)); TODO
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}
