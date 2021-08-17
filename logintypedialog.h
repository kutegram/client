#ifndef LOGINTYPEDIALOG_H
#define LOGINTYPEDIALOG_H

#include <QDialog>

namespace Ui {
class LoginTypeDialog;
}

class LoginTypeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginTypeDialog(QWidget *parent = 0);
    ~LoginTypeDialog();

public slots:
    void qrButton_clicked();
    void phoneButton_clicked();
    void buttonBox_rejected();
private:
    Ui::LoginTypeDialog *ui;
};

#endif // LOGINTYPEDIALOG_H
