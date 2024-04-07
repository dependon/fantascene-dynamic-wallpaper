#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>

namespace Ui {
class HelpDialog;
}

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog();

private Q_SLOTS:
    void on_closeBtn_clicked();

private:
    Ui::HelpDialog *ui;
};

#endif // HELPDIALOG_H
