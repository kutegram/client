#ifndef QRLOGINDIALOG_H
#define QRLOGINDIALOG_H

#include <QDialog>

namespace Ui {
class QRLoginDialog;
}

class QRLoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit QRLoginDialog(QString encode, QWidget *parent = 0);
    ~QRLoginDialog();
    
private:
    Ui::QRLoginDialog *ui;
};

#endif // QRLOGINDIALOG_H
