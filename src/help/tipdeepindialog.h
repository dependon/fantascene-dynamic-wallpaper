#ifndef TIPDEEPINDIALOG_H
#define TIPDEEPINDIALOG_H

#include <QDialog>

namespace Ui {
class TipDeepinDialog;
}

class TipDeepinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TipDeepinDialog(QWidget *parent = nullptr);
    ~TipDeepinDialog();

private:
    Ui::TipDeepinDialog *ui;
};

#endif // TIPDEEPINDIALOG_H
