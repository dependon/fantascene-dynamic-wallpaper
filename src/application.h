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

/* Translation file path */
#define TRANSALTION_PATH "/usr/share/fantascene-dynamic-wallpaper/translations"

#define dApp (static_cast<Application*>(QCoreApplication::instance()))

#define APP_VERSION "1.9.9"

#define MAINWINDOWWIDTH 1024
#define MAINWINDOWHEIGHT 700

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

#ifndef Q_OS_WIN
    xcb_connection_t *getXcb_connection_t();
    void  *getDisplay();
#endif
    //set app color
    void setAppColor(const QString & strColor);



    void load_translation_sys_files(const QString path);

    void load_translation_files(const QString path,const QString code);

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

    void sigSetDownloadIng(bool bRet);

    void sigDownloadError();

    // 设置时间控件字体的接口
    void setTimeFontFamily(const QString &fontFamily);
    // 设置时间控件字体大小的接口
    void setTimeFontPointSize(int size);
    // 设置时间控件字体颜色的接口
    void setTimeFontColor(const QColor &color);
    // 设置时间控件位置
    void setTimeMove(int,int);
    // 设置时间控件格式
    void setTimeFormat(const QString &format);
    // 设置时间控件显示
    void setTimeVisible(bool bVisble);
    // 设置时间控件透明度
    void setTimeTransparent(int value);
    // 设置视频播放插件
    void sigSetMeidaType(VideoWidgetType type);

    // 设置Cpu控件显示
    void setCpuVisible(bool bVisble);
    // 设置控件位置
    void setCpuMove(int,int);
    // 设置控件字体颜色的接口
    void setCpuFontColor(const QColor &color);
    // 设置控件透明度
    void setCpuTransparent(int value);

    // 设置内存控件显示
    void setMemoryVisible(bool bVisble);
    // 设置控件位置
    void setMemoryMove(int,int);
    // 设置控件字体颜色的接口
    void setMemoryFontColor(const QColor &color);
    // 设置控件透明度
    void setMemoryTransparent(int value);

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

    QTranslator *m_translator{nullptr};

};

#endif // APPLICATION_H
