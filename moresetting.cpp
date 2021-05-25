#include "moresetting.h"
#include "ui_moresetting.h"
#include "application.h"

MoreSetting::MoreSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoreSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("更多设置界面"));
}

MoreSetting::~MoreSetting()
{
    delete ui;
}

void MoreSetting::setData(const MoreSetData &data)
{
    if (data.isAuto == 0) {
        ui->autoBox->setCurrentText("是");
    } else {
        ui->autoBox->setCurrentText("否");
    }
    if (data.fps != 0) {
        ui->fpsbox->setCurrentText(QString::number(data.fps));
    } else {
        ui->fpsbox->setCurrentText("默认");
    }
    if(data.hwdec.contains("gpu")  ||
            data.hwdec.contains("vaapi")  ||
            data.hwdec.contains("vdpau")  ||
            data.hwdec.contains("no")  ||
            data.hwdec.contains("auto")
            ){
        ui->hwdecEdit->hide();
        ui->hwdecBox->setCurrentText(data.hwdec);
    }else {
        ui->hwdecBox->setCurrentText("其他");
        ui->hwdecEdit->setText(data.hwdec);
    }



}

void MoreSetting::on_okBtn_clicked()
{
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    if (ui->autoBox->currentText() == "是") {
        dApp->m_moreData.isAuto = 0;
    } else {
        dApp->m_moreData.isAuto = 1;
    }
    //    dApp->setMpvValue("correct-pts", "no");
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    QString str = ui->fpsbox->currentText();
    if (!str.contains("默认")) {
        dApp->setMpvValue("fps", ui->fpsbox->currentText());
    } else {
        dApp->setMpvValue("fps", "0");
    }

    QString hwdecBoxStr =ui->hwdecBox->currentText();
    if(hwdecBoxStr.contains("其他")){
        dApp->m_moreData.hwdec=ui->hwdecEdit->text();
    }
    else {
        dApp->m_moreData.hwdec=ui->hwdecBox->currentText();
    }

    emit dApp->moreSettingSave();
    close();
}

void MoreSetting::on_cancelBtn_clicked()
{
    close();
}

void MoreSetting::on_hwdecBox_activated(const QString &arg1)
{
    if(arg1.contains("其他")){
        ui->hwdecEdit->show();
    }
    else {
        ui->hwdecEdit->hide();
    }
}
