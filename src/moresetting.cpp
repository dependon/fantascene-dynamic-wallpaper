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

#include "ini/inimanager.h"
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

    if (data.vo == "libmpv"  ||
        data.vo == ""  ||
        data.vo == "opengl"  ||
        data.vo == "opengl-cb"
        ) 
	{
        ui->voEdit->hide();
        ui->voBox->setCurrentText(data.vo);
    } else {
        ui->voBox->setCurrentText(tr("other"));
        ui->voEdit->setText(data.vo);
    }


    if (data.isShowDesktopIcon) {
        ui->desktopShowCombox->setCurrentText(tr("true"));
    } else {
        ui->desktopShowCombox->setCurrentText(tr("false"));
    }

    if (data.isTop) {
        ui->topBox->setCurrentText(tr("true"));
    } else {
        ui->topBox->setCurrentText(tr("false"));
    }
    if(data.isEventPenetration)
    {
        ui->eventBox->setCurrentText(tr("true"));
    }
    else
    {
        ui->eventBox->setCurrentText(tr("false"));
    }
    ui->label_name->setText(dApp->m_currentDesktopName);
    ui->desktop_transparency->setValue(dApp->m_moreData.m_DesktopTransparency *100.0);
    ui->back_transparency->setValue(dApp->m_moreData.m_WallpaperTransparency *100.0);

    ui->fontBox->setCurrentText(dApp->m_moreData.fontColor);

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

    QString voBoxStr = ui->voBox->currentText();
    if (voBoxStr.contains(tr("other"))) {
        dApp->m_moreData.vo = ui->voEdit->text();
    } else {
        dApp->m_moreData.vo = ui->voBox->currentText();
    }

    QString isDesktopIcon = ui->desktopShowCombox->currentText();
    if (isDesktopIcon.contains(tr("true"))) {
        dApp->m_moreData.isShowDesktopIcon = true;
    } else {
        dApp->m_moreData.isShowDesktopIcon = false;
    }

    QString isTop = ui->topBox->currentText();
    if (isTop.contains(tr("true"))) {
        if(dApp->m_moreData.isTop!= true)
        {
            Q_EMIT dApp->sigWallpaperTopChanged(true);
        }
        dApp->m_moreData.isTop = true;
    } else {
        if(dApp->m_moreData.isTop!= false)
        {
            Q_EMIT dApp->sigWallpaperTopChanged(false);
        }
        dApp->m_moreData.isTop = false;
    }
    QString isEvent = ui->eventBox->currentText();
    if (isEvent.contains(tr("true"))) {
        if(dApp->m_moreData.isEventPenetration!= true)
        {
            Q_EMIT dApp->sigWallpaperEventChanged(true);
        }
        dApp->m_moreData.isEventPenetration = true;
    } else {
        if(dApp->m_moreData.isEventPenetration!= false)
        {
            Q_EMIT dApp->sigWallpaperEventChanged(false);
        }
        dApp->m_moreData.isEventPenetration = false;
    }
    dApp->m_moreData.fontColor = ui->fontBox->currentText();

    Q_EMIT dApp->moreSettingSave();

    dApp->setSpecialDesktop();

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


void MoreSetting::on_desktop_transparency_valueChanged(int value)
{
    double dop = (double)value/100.0;
    dApp->changePidOpacity(dop);
    dApp->m_moreData.m_DesktopTransparency = dop;
    IniManager::instance()->setValue("WallPaper/DesktopTransparency",QString::number(dop));

}

void MoreSetting::on_back_transparency_valueChanged(int value)
{
    double dop = (double)value/100.0;
    dApp->changeMeOpacity(dop);
    dApp->m_moreData.m_WallpaperTransparency = dop;
    IniManager::instance()->setValue("WallPaper/WallpaperTransparency",QString::number(dop));
}

void MoreSetting::on_voBox_currentTextChanged(const QString &arg1)
{
    if (arg1.contains(tr("other"))) {
        ui->voEdit->show();
    } else {
        ui->voEdit->hide();
    }
}

