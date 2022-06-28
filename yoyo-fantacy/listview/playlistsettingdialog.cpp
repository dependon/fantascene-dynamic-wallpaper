#include "playlistsettingdialog.h"
#include "ui_playlistsettingdialog.h"

#include "application.h"

PlaylistSettingDialog::PlaylistSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaylistSettingDialog)
{
    ui->setupUi(this);
    ui->timeEdit->setVisible(ui->timeCombox->currentText().contains(tr("other")));
    ui->timeEdit->setText(QString::number(dApp->m_PlaylistTimer));
    ui->timeCombox->setCurrentText(QString::number(dApp->m_PlaylistTimer));
    if (ui->timeCombox->currentText() != QString::number(dApp->m_PlaylistTimer)) {
        ui->timeCombox->setCurrentText(tr("other"));
    }

}

PlaylistSettingDialog::~PlaylistSettingDialog()
{
    delete ui;
}

void PlaylistSettingDialog::on_timeCombox_currentTextChanged(const QString &arg1)
{
    if (!ui->timeCombox->currentText().contains(tr("other"))) {
        ui->timeEdit->setText(arg1);
    }
    ui->timeEdit->setVisible(arg1.contains(tr("other")));
}

void PlaylistSettingDialog::on_okBtn_clicked()
{
    dApp->setPlayListTimer(ui->timeEdit->text().toInt());
    dApp->saveSetting();

    close();
}

void PlaylistSettingDialog::on_cancelBtn_clicked()
{
    close();
}
