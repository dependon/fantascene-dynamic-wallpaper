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
#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QObject>
#include <QThread>

#include <QRect>
#include <QMutex>
#include <QVector>
#include <QDir>

#include "data.h"

const QString PIC_DIR_PATH =   QDir::homePath() +
                               "/.config/fantascene-dynamic-wallpaper/.thumbnail";

#define dApp (static_cast<Application*>(QCoreApplication::instance()))

class xcb_connection_t;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    const QString thumbnailCachePath();
    const QPixmap getThumbnail(const QString &path);
    const QPixmap reloadThumbnail(const QString &path);
    const QPixmap setNewThumbnail(const QString &path,const QString &thumbnailPath);
    QString getThumbnailPath(const QString &path);
    const QPixmap getThumbnailText(const QString &path);

    void setDesktopTransparent();

    bool setThumbnail(const QString &path);

    void setPlayListTimer(int s);
    void setisPlayList(bool bRet);

    //特殊桌面的处理
    void setSpecialDesktop();

    bool addLocalPaths(QStringList strList);
    bool removeLocalPaths(QStringList strList);
    bool clearLocalPaths();
    bool addPlayListaths(QStringList strList);
    bool removePlayListPaths(QStringList strList);
    bool clearPlayListPaths();

    //检查配置,寻求最大帮助
    void CheckSystem();

    //改变原始的透明度
    void changePidOpacity(const double &opacity );

    //改变自身进程的透明度
    void changeMeOpacity(const double &opacity );

    QSet<QString> convertQStringListToSet(const QStringList &list);

    xcb_connection_t *getXcb_connection_t();
    void  *getDisplay();

    //set app color
    void setAppColor(const QString & strColor);

Q_SIGNALS:
    void setPlayPath(const QString &PATH);
    void setPlayPath2(const QString &PATH);
    void setMpvPlay();
    void setMpvpause();
    void setMpvstop();
    void setMpvVolume(const qint32 volume);
    void setMpvValue(const QString &key, const QString &value);

    void setScreen(const int &index);
    void pathChanged(const QString &);
    void pathChanged2(const QString &);
    void refreshPix(const QPixmap &);
    void setScreenMode(const QString &);
    void sigupdateGeometry();
    void quitApp();

    void setWallPaper(const QString &path);
    void setWallPaper2(const QString &path);
    void saveSetting();

    void addPaperView(const QString &path);//加入全部播放列表

    void addplaylist(const QString &path);//加入自定义播放列表

    void removePaperView(const QString &path);

    void sigscreenshot();

    void moreSettingSave();

    void sigActiveWindow();

    void sigDesktopActive();

    void sigSetTransparency(const int value);

    void sigReadPlayerConfig();

    void sigWallpaperAction();

    void sigWallpaperTopChanged(bool);

    void sigWallpaperEventChanged(bool);

public:

    QRect m_manual{0, 0, 1, 1};
    ScreenMode m_cuurentMode{IdCopyScreen};
    int m_currentScreenNum{0};

    QStringList m_allPath;
    QStringList m_playlistPath;
    QMutex mutex;

    QVector <unsigned long> m_screenWid;
    QVector <unsigned long> m_screenDesktopWid;
    bool m_isNoMpvPause{true};
    bool m_isPlayList{false};
    int m_PlaylistTimer{600}; //s
    QTimer *m_pplaylistTimer{nullptr};

    MoreSetData m_moreData;
//    int m_isAutoMode{0};
    QString m_currentPath; //主屏壁纸
    bool m_isPath2{false};  //是否副屏显示其他壁纸
    QString m_currentPath2; //副屏壁纸

    bool m_currentIsPlay{false};


    QString m_wallpaperEnginePath;

    QString m_currentDesktopName;

    QList<QWindow> m_DesktopWindow;

    bool m_isLingMo =false;//是否是LingMo
    bool m_isUKUI =false;//是否是ukui
    bool m_isDDE23 =false;//是否是dde23
    bool m_isGXDE = false;//是否是GXDE
    QThread * m_startDesktop = nullptr;

    int m_onlineCount{0};//当前搜索的总个数

};

#endif // APPLICATION_H
