#include "logintypedialog.h"
#include "ui_logintypedialog.h"

LoginTypeDialog::LoginTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginTypeDialog)
{
    ui->setupUi(this);
}

LoginTypeDialog::~LoginTypeDialog()
{
    delete ui;
}

void LoginTypeDialog::qrButton_clicked()
{
    done(2);
}

void LoginTypeDialog::phoneButton_clicked()
{
    done(3);
}

void LoginTypeDialog::buttonBox_rejected()
{
    reject();
}
