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
#ifndef PLAYLISTSETTINGDIALOG_H
#define PLAYLISTSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class PlaylistSettingDialog;
}

class PlaylistSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistSettingDialog(QWidget *parent = nullptr);
    ~PlaylistSettingDialog();

private Q_SLOTS:
    void on_timeCombox_currentTextChanged(const QString &arg1);

    void on_okBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::PlaylistSettingDialog *ui;
};

#endif // PLAYLISTSETTINGDIALOG_H
