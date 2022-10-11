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
#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
class view;
namespace Ui {
class historyWidget;
}

class historyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit historyWidget(QWidget *parent = nullptr);
    ~historyWidget();
protected:
    void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void on_setWallPaper_clicked();

    void on_delWallPaper_clicked();

    void on_importBtn_clicked();

    void on_addPlaylistBtn_clicked();

    void on_moreSetting_clicked();

    void on_loopBox_clicked(bool checked);

    void on_deletePlaylist_clicked();

private:
    Ui::historyWidget *ui;
    view *m_viewHistory{nullptr};
    view *m_viewPlayList{nullptr};
};

#endif // HISTORYWIDGET_H
