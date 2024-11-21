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
#ifndef MORESETTING_H
#define MORESETTING_H

#include <QWidget>
#include <QMap>
#include "data.h"
namespace Ui {
class MoreSetting;
}

class MoreSetting : public QWidget
{
    Q_OBJECT

public:
    explicit MoreSetting(QWidget *parent = nullptr);
    ~MoreSetting();

    void setData(const MoreSetData &data);

    void setLanguageCombox();

protected:
    void changeEvent(QEvent *event);
private Q_SLOTS:
    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

    void on_hwdecBox_activated(const QString &arg1);

    void on_desktop_transparency_valueChanged(int value);

    void on_back_transparency_valueChanged(int value);

    void on_voBox_currentTextChanged(const QString &arg1);

    void on_check_deepin_stateChanged(int arg1);

    void on_setWorkPaper_clicked();

    void on_setWorkPaper_2_clicked();

private:
    Ui::MoreSetting *ui;
    QMap<QString, QString> m_languageMap;
};

#endif // MORESETTING_H
