#include "historywindow.h"
#include "ui_historywindow.h"

#include "historyitemdelegate.h"
#include "library/telegramclient.h"

HistoryWindow::HistoryWindow(TelegramClient *client, TLInputPeer input, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow),
    flickcharm(),
    client(client),
    model(0)
{
    ui->setupUi(this);

    flickcharm.activateOn(ui->historyView);

    ui->historyView->setModel(model = new HistoryItemModel(client, input, ui->historyView));
    ui->historyView->setItemDelegate(new HistoryItemDelegate(ui->historyView));
}

HistoryWindow::~HistoryWindow()
{
    delete ui;
}

void HistoryWindow::sendButton_clicked()
{
    if (ui->messageEdit->text().isEmpty()) return;
    client->sendMessage(model->peer, ui->messageEdit->text());
    ui->messageEdit->clear();
}
