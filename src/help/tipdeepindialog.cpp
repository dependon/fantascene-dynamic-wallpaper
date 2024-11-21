#include "tipdeepindialog.h"
#include "ui_tipdeepindialog.h"

TipDeepinDialog::TipDeepinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TipDeepinDialog)
{
    ui->setupUi(this);
}

TipDeepinDialog::~TipDeepinDialog()
{
    delete ui;
}
