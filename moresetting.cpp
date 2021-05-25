#include "moresetting.h"
#include "ui_moresetting.h"
#include "application.h"

MoreSetting::MoreSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoreSetting)
{
    ui->setupUi(this);
}

MoreSetting::~MoreSetting()
{
    delete ui;
}

void MoreSetting::setData(const MoreSetData &data)
{
    if (data.bisAuto) {
        ui->autoBox->setCurrentText("是");
    } else {
        ui->autoBox->setCurrentText("否");
    }
    ui->fpsbox->setCurrentText(QString::number(data.fps));


}

void MoreSetting::on_okBtn_clicked()
{
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    if (ui->autoBox->currentText() == "是") {
        dApp->m_moreData.bisAuto = true;
    } else {
        dApp->m_moreData.bisAuto = false;
    }
//    dApp->setMpvValue("correct-pts", "no");
    dApp->setMpvValue("fps", ui->fpsbox->currentText());
    close();
}

void MoreSetting::on_cancelBtn_clicked()
{
    close();
}
