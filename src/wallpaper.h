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
#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "mediapluginwidget.h"
#include "data.h"

#include <QTimer>
#include <QWidget>
#include <QHBoxLayout>
#include <QDateTime>

#include "webwidget.h"
#include "desktop/iconview.h"

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif
class TimeDisplayWidget;
class MemoryMonitorWidget;
class CpuMonitorWidget;
class NetworkMonitorWidget;

class QLabel;
//class Desktop;
class Wallpaper : public QWidget
{
    Q_OBJECT
public:
    explicit Wallpaper(QString path = nullptr, int currentScreen = 0, QWidget *parent = nullptr);

    void changeScreenMode(ScreenMode mode);
    void LeftMouseMove(QWidget *eventsReciverWidget, QPoint clickPos);
    void LeftMouseRelease(QWidget *eventsReciverWidget, QPoint clickPos);
    void LeftMousePress(QWidget *eventsReciverWidget, QPoint clickPos);

    void setIconVisble(bool visble);
    void refreashLayout();
public Q_SLOTS:
    void setFile(const QString &path);
    void setFile2(const QString &path);
    void setVolume(const qint32 volume);
    void setScreen(const int &index);
    void clear();
    void play();
    void pause();
    void stop();

    void slotrefreshPix(const QPixmap &pix);
    void slotsetScreenMode(const QString &mode);
    void updateGeometry();

    void slotMouseEvent();
    void slotMouseClick(const int &index);

    void slotActiveWallpaper(bool bRet);
    void slotWallpaperEventChanged(bool bRet);

    void setTimeVisible (bool bVisible );
    void setCpuVisible (bool bVisible );
    void setMemoryVisible (bool bVisible );
    void setNetworkVisible(bool bVisible );

private:
    void registerDesktop();
    bool event(QEvent *event) override;

private Q_SLOTS:
    void onSysLockState(QString, QVariantMap key2value, QStringList);

    void slotSetMpvValue(const QString &key, const QString &value);

    void slotSetTransparency(const int value);
private:
    QHBoxLayout *mainlayout;

    MediaPluginWidget *m_media{nullptr};
    MediaPluginWidget *m_media2{nullptr};

    int m_currentScreen{0};
    QLabel *m_label2{nullptr};

    webWidget *m_webView{nullptr};
    webWidget *m_webView2{nullptr};
    QTimer *m_mouseWebEventTimer{nullptr};
    QPoint m_currentPos{0, 0};
//    ScreenMode m_cuurentMode{IdCopyScreen};
//    Desktop *de{nullptr};
    IconView *m_iconView{nullptr};

    qint64 m_lastHideWallpaper = 0;//用于区分上次隐藏的时间，用于关闭程序
    bool m_quitApp=false;

    TimeDisplayWidget * m_timedisplayWidget{nullptr};
    bool m_TimeVisible{false};
    CpuMonitorWidget * m_cpudisplayWidget{nullptr};
    bool m_CpuVisible{false};
    MemoryMonitorWidget * m_memorydisplayWidget{nullptr};
    bool m_MemoryVisible{false};
    NetworkMonitorWidget * m_networkMonitorWidget{nullptr};
    bool m_NetworkVisible{false};

#ifdef Q_OS_WINDOWS
    HHOOK hook;
    HWND workerW;
#endif
};

#endif // WALLPAPER_H
