/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <DMainWindow>
#include <QVBoxLayout>
#include <QListWidget>
#include <dswitchbutton.h>

DWIDGET_USE_NAMESPACE

class Settings : public DMainWindow
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);

signals:
    void requestSetFolder(const QStringList &list, const bool isVideo);
    void requestSetVolume(const qint32 volume);
    void requestSetInterval(const qint32 interval);
    void requestClear();
    void requestPlay();
    void requestPause();
    void requestStop();

private:
    void addOne();
    void removeOne();
    void clean();
    void finished();

private:
    QVBoxLayout *m_layout;
    QListWidget *m_listView;
    QStringList m_list;
    DSwitchButton *m_videoSwitchBtn;
};

#endif // SETTINGS_H
