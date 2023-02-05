/*
 * Copyright (C) 2020 ~ 2022 LiuMingHang.
 *
 * Author:     LiuMingHang <liuminghang0821@gmail.com>
 *
 * Maintainer: LiuMingHang <liuminghang0821@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "moresetting.h"
#include "ui_moresetting.h"
#include "application.h"

#include <QTimer>
#include <QDesktopWidget>

MoreSetting::MoreSetting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MoreSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Advanced Settings"));
//    ui->label->hide();
//    ui->autoBox->hide();
}

MoreSetting::~MoreSetting()
{
    delete ui;
}

void MoreSetting::setData(const MoreSetData &data)
{
    if (data.isAuto == 0) {
        ui->autoBox->setCurrentText(tr("continue playing"));
    } else {
        ui->autoBox->setCurrentText(tr("pause"));
    }
    if (data.fps != 0) {
        ui->fpsbox->setCurrentText(QString::number(data.fps));
    } else {
        ui->fpsbox->setCurrentText(tr("default"));
    }
    if (data.hwdec.contains("gpu")  ||
            data.hwdec.contains("vaapi")  ||
            data.hwdec.contains("vdpau")  ||
            data.hwdec.contains("none")  ||
            data.hwdec.contains("auto")
       ) {
        ui->hwdecEdit->hide();
        ui->hwdecBox->setCurrentText(data.hwdec);
    } else {
        ui->hwdecBox->setCurrentText(tr("other"));
        ui->hwdecEdit->setText(data.hwdec);
    }

    if (data.isShowDesktopIcon) {
        ui->desktopShowCombox->setCurrentText(tr("true"));
    } else {
        ui->desktopShowCombox->setCurrentText(tr("false"));
    }

}

void MoreSetting::on_okBtn_clicked()
{
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    if (ui->autoBox->currentText() == tr("continue playing")) {
        dApp->m_moreData.isAuto = 0;
    } else {
        dApp->m_moreData.isAuto = 1;
    }
    //    dApp->setMpvValue("correct-pts", "no");
    dApp->m_moreData.fps = ui->fpsbox->currentText().toInt();
    QString str = ui->fpsbox->currentText();
    if (!str.contains(tr("default"))) {
        dApp->setMpvValue("fps", ui->fpsbox->currentText());
    } else {
        dApp->setMpvValue("fps", "0");
    }

    QString hwdecBoxStr = ui->hwdecBox->currentText();
    if (hwdecBoxStr.contains(tr("other"))) {
        dApp->m_moreData.hwdec = ui->hwdecEdit->text();
    } else {
        dApp->m_moreData.hwdec = ui->hwdecBox->currentText();
    }

    QString isDesktopIcon = ui->desktopShowCombox->currentText();
    if (isDesktopIcon.contains(tr("true"))) {
        dApp->m_moreData.isShowDesktopIcon = true;
    } else {
        dApp->m_moreData.isShowDesktopIcon = false;
    }

    Q_EMIT dApp->moreSettingSave();
    close();
}

void MoreSetting::on_cancelBtn_clicked()
{
    close();
}

void MoreSetting::on_hwdecBox_activated(const QString &arg1)
{
    if (arg1.contains(tr("other"))) {
        ui->hwdecEdit->show();
    } else {
        ui->hwdecEdit->hide();
    }
}

