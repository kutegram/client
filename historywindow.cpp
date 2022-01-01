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
#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    optionsAction(this),
#endif
    backAction(this)
{
    ui->setupUi(this);
#if QT_VERSION >= 0x040702
    ui->messageEdit->setPlaceholderText(QApplication::translate("HistoryWindow", "Type a message...", 0, QApplication::UnicodeUTF8));
#endif

#if defined(Q_OS_SYMBIAN) && (QT_VERSION < 0x040800)
    /**
      * Qt installer for Symbian does not contain translation files (*.qm):
      * https://bugreports.qt.io/browse/QTBUG-4919
      * Create a custom "Options" softkey and localize it on the application level.
      */
    optionsAction.setText(QApplication::translate("HistoryWindow", "Options", 0, QApplication::UnicodeUTF8));
    QMenu* optionsMenu = new QMenu(this);
    // TODO: add actions for history window
    optionsAction.setMenu(optionsMenu);
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        optionsAction.setIcon(QIcon(":/icons/options.svg"));
    }
    optionsAction.setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(&optionsAction);
#endif

    connect(&backAction, SIGNAL(triggered()), this, SLOT(backAction_triggered()));
    backAction.setText(QApplication::translate("HistoryWindow", "Back", 0, QApplication::UnicodeUTF8));

#ifdef Q_OS_SYMBIAN
    // Softkeys with icon are on Symbian Belle and higher
    if (QSysInfo::s60Version() > QSysInfo::SV_S60_5_2)
    {
        backAction.setIcon(QIcon(":/icons/back.svg"));
    }
#endif

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
