#include "historywindow.h"
#include "ui_historywindow.h"

#include "historyitemdelegate.h"
#include "library/telegramclient.h"
#include <QScrollBar>

HistoryWindow::HistoryWindow(TelegramClient *client, TLInputPeer input, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistoryWindow),
    flickcharm(),
    client(client),
    model(0),
    backAction(this)
{
    ui->setupUi(this);
#if QT_VERSION >= 0x040702
    ui->messageEdit->setPlaceholderText(QApplication::translate("HistoryWindow", "Type a message...", 0, QApplication::UnicodeUTF8));
#endif

    connect(&backAction, SIGNAL(triggered()), this, SLOT(backAction_triggered()));
    backAction.setText(QApplication::translate("HistoryWindow", "Back", 0, QApplication::UnicodeUTF8));
    backAction.setIcon(QIcon(":/icons/back.svg"));
    backAction.setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(&backAction);

    flickcharm.activateOn(ui->historyView);

    ui->historyView->setModel(model = new HistoryItemModel(client, input, ui->historyView));
    ui->historyView->setItemDelegate(new HistoryItemDelegate(ui->historyView));

    if (model->canFetchMoreUpwards(QModelIndex())) model->fetchMoreUpwards(QModelIndex());
    if (model->canFetchMore(QModelIndex())) model->fetchMore(QModelIndex());
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

void HistoryWindow::backAction_triggered()
{
    close();
}
