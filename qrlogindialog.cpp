#include "qrlogindialog.h"
#include "ui_qrlogindialog.h"

#include <QPixmap>
#include <QPainter>
#include "libqrencode/qrencode.h"

QRLoginDialog::QRLoginDialog(QString encode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRLoginDialog)
{
    ui->setupUi(this);

    QRcode* code = QRcode_encodeString(encode.toStdString().c_str(), 0, QR_ECLEVEL_Q, QR_MODE_8, 1);

    if (code) {
        QImage result(code->width + 8, code->width + 8, QImage::Format_RGB32);
        result.fill(Qt::white);

        QPainter painter;
        painter.begin(&result);

        painter.setPen(Qt::black);
        for (qint32 y = 0; y < code->width; ++y) {
            for (qint32 x = 0; x < code->width; ++x) {
                if (code->data[y * code->width + x] & 1) painter.drawPoint(x + 4, y + 4);
            }
        }

        painter.end();

        result = result.scaled(code->width * 4, code->width * 4);
        QRcode_free(code);

        ui->qrLabel->setPixmap(QPixmap::fromImage(result));
    }
}

QRLoginDialog::~QRLoginDialog()
{
    delete ui;
}
