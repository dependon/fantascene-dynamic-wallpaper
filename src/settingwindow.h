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
#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include "application.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QWindow>
#include <QTimer>

#include <QtX11Extras/QX11Info>
#include <qnamespace.h>
#include <xcb/xcb_ewmh.h>
#include "wallpaper.h"

#include "dbuswallpaperservice.h"
class QSystemTrayIcon;
class QMenu;
class historyWidget;
class QThread;
class MoreSetting;
class wallpaperEnginePlugin;

namespace Ui {
class settingWindow;
}

class settingWindow : public QWidget
{
    Q_OBJECT

public:
    explicit settingWindow(QWidget *parent = nullptr, QMainWindow *mainWindow = nullptr);
    ~settingWindow();

    void readSettings();

    void executeSettings();

    QString getCurrentPath();

    int getCurrentNumber();

    int isAutoStart();

    QVector<WId> currentWorkWindow();

    void setScreenMode(const QString &arg);

    //获取当前工作的窗口wid
    QVector<uint> getCurrentWorkspaceWindows();
    static xcb_atom_t internAtom(xcb_connection_t *connection, const char *name, bool only_if_exists = true);
    QVector<xcb_window_t> getWindows() const;
    qint32 getWorkspaceForWindow(quint32 WId);

    //通过wid获取窗口尺寸
    QRect geometry(WId id) const;
    //通过wid获取桌面状态
    Qt::WindowState getWindowState(WId id) ;
    //初始化atom
    void initAtom();
    //获得窗口属性,桌面，普通窗口等
    uint32_t searchWindowType(int wid);

    void initWallpaperWidget();
protected:
    bool eventFilter(QObject *obj, QEvent *event);
private Q_SLOTS:

    void slotMoreSettingSave();

    void saveSettings();

    void on_pathBtn_clicked();

    void on_setBtn_clicked();

    void on_cancelBtn_clicked();

    void on_pauseBtn_clicked();

    void on_stopBtn_clicked();

    void on_Slider_valueChanged(int value);

    void on_startBtn_clicked();

    void on_startScreen_clicked();

    void on_autoStart_clicked();

    void on_noAutoStart_clicked();

    void on_autoStartBox_stateChanged(int arg1);

    void pathChanged(const QString &path);
    void on_comboBox_activated(const QString &arg1);

    void on_setManual_clicked();

    void quitApp();

    void on_mainWeb_clicked();

    void on_history_clicked();

    void slotWallPaper(const QString &path);


    void on_giteeWeb_clicked();

    void on_githubWeb_clicked();

    void on_bugBtn_clicked();

    void on_videoBLCombox_activated(const QString &arg1);

    void on_videoBZDY_clicked();

    void on_pathEdit_textChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);


    void on_moreSettingBtn_clicked();

    void on_pluginBtn_clicked();
    void on_tansparency_slider_valueChanged(int value);

    void slotShowDesktopIcon(bool isIcon);

    void slotTimerSaveSettings();

public Q_SLOTS:
    void activeWindow();
private:
    Ui::settingWindow *ui;
    QSystemTrayIcon *m_trayIcon{nullptr};
    QMenu *m_traymenu{nullptr};
    QMainWindow *m_parentMainWindow{nullptr};
    int index = 1;

    int m_crrenNumber{0};
    int m_isAutoStart{0};
    QString m_currentMode;
    int m_voiceVolume{0};

    historyWidget *m_history{nullptr};

    wallpaperEnginePlugin *m_wallpaperEnginePlugin{nullptr};

    QString m_videoASpectStr{"默认"};
    double m_videoAspect{1.33};

    QThread *m_x11thread{nullptr};
    bool m_stopx11Thread{false};

    QMenu *m_aboutMenu{nullptr};
    MoreSetting *m_moreSetting{nullptr};

    QVector<WId> m_windowList;

    xcb_ewmh_connection_t m_ewmh_connection;

    QMutex m_mutex;

    Wallpaper *m_wallpaper{nullptr};
    QTimer *m_timerSave{nullptr};
};

#endif // SETTINGWINDOW_H
