#include "qrlogindialog.h"
#include "ui_qrlogindialog.h"

#include <QPixmap>

QRLoginDialog::QRLoginDialog(QImage& qrImage, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRLoginDialog)
{
    ui->setupUi(this);
    ui->qrLabel->setPixmap(QPixmap::fromImage(qrImage));
}

QRLoginDialog::~QRLoginDialog()
{
    delete ui;
}
