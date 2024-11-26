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
#include "wallpaper.h"

#ifdef Q_OS_LINUX
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <malloc.h>
#endif

#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>

#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QGraphicsOpacityEffect>

#include <QDebug>
#include <QLabel>
#include <QDBusConnection>

#include <QGuiApplication>
#include <QEvent>
#include <QMouseEvent>
#include "ini/inimanager.h"
#ifdef Q_OS_LINUX
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include <X11/Xatom.h>
#include <X11/Xproto.h>
//#include <X11/extensions/shape.h>
//#include <X11/extensions/Xrender.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <iostream>
#include <QStandardPaths>
#include <QScreen>
#ifdef USE_WEBENGINE

#include <QWebEngineView>
#endif

#include "application.h"

Wallpaper::Wallpaper(QString path, int currentScreen, QWidget *parent)
    : QWidget(parent)
    , m_currentScreen(currentScreen)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    registerDesktop();

    connect(dApp, &Application::refreshPix, this, &Wallpaper::slotrefreshPix);
    connect(dApp, &Application::setScreenMode, this, &Wallpaper::slotsetScreenMode);

    connect(dApp, &Application::setPlayPath, this, &Wallpaper::setFile);
    connect(dApp, &Application::setPlayPath2, this, &Wallpaper::setFile2);
    connect(dApp, &Application::setMpvPlay, this, &Wallpaper::play);
    connect(dApp, &Application::setMpvpause, this, &Wallpaper::pause);
    connect(dApp, &Application::setMpvstop, this, &Wallpaper::stop);
    connect(dApp, &Application::setMpvVolume, this, &Wallpaper::setVolume);
    connect(dApp, &Application::setScreen, this, &Wallpaper::setScreen);
    connect(dApp, &Application::sigupdateGeometry, this, &Wallpaper::updateGeometry);
    connect(dApp, &Application::setMpvValue, this, &Wallpaper::slotSetMpvValue);

    connect(dApp, &Application::sigSetTransparency, this, &Wallpaper::slotSetTransparency);
    connect(dApp,&Application::sigWallpaperTopChanged,this,&Wallpaper::slotActiveWallpaper);
    connect(dApp,&Application::sigWallpaperEventChanged,this,&Wallpaper::slotWallpaperEventChanged);


    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    // 监听屏幕大小变化信号
    QObject::connect(primaryScreen, &QScreen::geometryChanged, [=]() {
        QTimer::singleShot(1000, [ = ]{
            updateGeometry();
        });
        updateGeometry();
    });

    QDBusConnection::sessionBus().connect("com.deepin.SessionManager", "/com/deepin/SessionManager",
                                          "org.freedesktop.DBus.Properties", "PropertiesChanged", this,
                                          SLOT(onSysLockState(QString, QVariantMap, QStringList)));

    m_mouseWebEventTimer = new QTimer(this);
    connect(m_mouseWebEventTimer, SIGNAL(timeout()), this, SLOT(slotMouseEvent()));
    m_mouseWebEventTimer->start(50);

    QString paths = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
#ifdef Q_OS_LINUX
    m_iconView = new IconView(0, paths, this);
    connect(m_iconView, &IconView::sigMouseClick, this, &Wallpaper::slotMouseClick);
    m_iconView->move(0, 0);
#endif
    QTimer::singleShot(1, this, &Wallpaper::updateGeometry);
    QTimer::singleShot(1000, this, [ = ] {
        int index = 0;
        int index1 = 0;
        for (const QString &arg : qApp->arguments())
        {
            if (index != 0) {
                if (QFileInfo(arg).isFile()) {
                    setFile(arg);
                    setFile2(arg);
                    play();
                    index1++;
                }
            }
            index++;
        }
        if (index1 == 0)
        {

            QString playPath = QApplication::applicationDirPath()+"/normal.mp4";
            if(!QFile::exists(playPath))
            {
                playPath = "/usr/share/fantascene-dynamic-wallpaper/normal/normal.mp4";
            }
            if(!QFile::exists(playPath))
            {
                playPath = "/usr/share/fantascene-dynamic-wallpaper/normal/deepin/normal.mp4";
            }
            dApp->m_currentPath = dApp->m_currentPath.replace("file://", "");
            if (!dApp->m_currentPath.isEmpty()) {
                if (QFileInfo(dApp->m_currentPath).isFile())
                {
                    playPath = dApp->m_currentPath;
                }
                else if (path.contains("www") || path.contains("http//") || path.contains("https//"))
                {
                    playPath = dApp->m_currentPath;
                }
                else
                {

                }
            }
            setFile(playPath);
            if(QGuiApplication::screens().size() > 1 && !dApp->m_currentPath2.isEmpty() && dApp->m_isPath2)
            {
                QString playPath2;
                dApp->m_currentPath2 = dApp->m_currentPath2.replace("file://", "");
                if (!dApp->m_currentPath2.isEmpty()) {
                    if (QFileInfo(dApp->m_currentPath2).isFile())
                    {
                        playPath2 = dApp->m_currentPath2;
                    }
                    else if (path.contains("www") || path.contains("http//") || path.contains("https//"))
                    {
                        playPath2 = dApp->m_currentPath2;
                    }
                }
                setFile2(playPath2);
            }
            else if (QGuiApplication::screens().size() > 1)
            {
                setFile2(playPath);
            }
            play();
            dApp->m_currentPath = QFileInfo(dApp->m_currentPath).filePath();
            Q_EMIT dApp->pathChanged(dApp->m_currentPath);

            slotActiveWallpaper(dApp->m_moreData.isTop);
        }
//        QTimer::singleShot(100, [ = ] {
//            updateGeometry();
//        });
    });

    setVolume(0);
    if(IniManager::instance()->contains("Wallpaper/EventPenetration"))
    {
        bool bEvent = IniManager::instance()->value("Wallpaper/EventPenetration").toBool();
        if(bEvent)
        {
            slotWallpaperEventChanged(bEvent);
        }
    }


}

void Wallpaper::changeScreenMode(ScreenMode mode)
{
    switch (mode) {
    case IdCopyScreen: {
        if (QGuiApplication::screens().size() > 1) {
            QString path = dApp->m_currentPath;
            if(dApp->m_isPath2)
            {
                path = dApp->m_currentPath2;
            }
            setFile2(path);
        }
        break;
    }
    case IdlayoutScreen: {
        if (nullptr != m_webView2) {
            layout()->removeWidget(m_webView2);
            delete m_webView2 ;
            m_webView2 = nullptr;
        }
        if (nullptr != m_mpv2) {
            layout()->removeWidget(m_mpv2);
            delete m_mpv2 ;
            m_mpv2 = nullptr;
        }
        break;
    }
    case IdManualSet: {
        if (nullptr != m_webView2) {
            layout()->removeWidget(m_webView2);
            delete m_webView2 ;
            m_webView2 = nullptr;
        }
        if (nullptr != m_mpv2) {
            layout()->removeWidget(m_mpv2);
            delete m_mpv2 ;
            m_mpv2 = nullptr;
        }
        break;
    }
    default:
        break;
    }
    //暂时调用两次,为保证切换顺利
    QTimer::singleShot(10, [ = ] {
        updateGeometry();
    });
    updateGeometry();
}

void Wallpaper::setScreen(const int &index)
{
    m_currentScreen = index;
}

void Wallpaper::setFile(const QString &path)
{
    dApp->m_currentPath = path;
    if (m_iconView) {
        m_iconView->setParent(this);
    }
#ifdef Q_OS_LINUX
    malloc_trim(0);
#endif
    if (path.contains("html") || path.contains("www") || path.contains("http//") || path.contains("https//")) {
        if (m_mpv) {
            layout()->removeWidget(m_mpv);
            m_mpv->setProperty("pause", true);
            delete m_mpv;
            m_mpv = nullptr;

        }
        if (!m_webView) {
            m_webView = new webWidget(this);
            m_webView->setContextMenuPolicy(Qt::NoContextMenu);
        }
        //        layout()->addWidget(m_webView);
        if (QFileInfo(path).isFile()) {
            m_webView->load(QUrl("file://" + path));
        } else {
            m_webView->load(QUrl(path));
        }

        m_webView->show();
        pause();
    }  else {
        if (m_webView) {
            layout()->removeWidget(m_webView);
            delete m_webView;
            m_webView = nullptr;
        }
        if (!m_mpv) {
            m_mpv = new MpvWidget(this);
            m_mpv->setGeometry(geometry());

            m_mpv->setProperty("loop", true);
            m_mpv->setProperty("panscan", 1);
            m_mpv->setGeometry(geometry());
            m_mpv->show();
        }

        m_mpv->command(QStringList() << "loadfile" << path);
        m_mpv->setProperty("pause", true);
    }
    //        //发送读取配置文件
    Q_EMIT dApp->sigReadPlayerConfig();

    refreashLayout();
}

void Wallpaper::setFile2(const QString &path)
{
    if (path.contains("html") || path.contains("www") || path.contains("http//") || path.contains("https//")) {
        if (qApp->screens().count() > 1 && dApp->m_cuurentMode == IdCopyScreen) {

            QString path = dApp->m_currentPath;
            if (m_webView) {
                if (!m_webView2) {
                    m_webView2 = new webWidget(this);
                    m_webView2->setContextMenuPolicy(Qt::NoContextMenu);
                }

                if (QFileInfo(path).isFile()) {
                    m_webView2->load(QUrl("file://" + path));
                } else {
                    m_webView2->load(QUrl(path));
                }
                m_webView2->show();
            }
        }
    }  else {
        if (m_webView2) {
            layout()->removeWidget(m_webView2);
            delete m_webView2;
            m_webView2 = nullptr;
        }
        if (qApp->screens().count() > 1 && IdCopyScreen == dApp->m_cuurentMode) {
            if (!m_mpv2) {
                m_mpv2 = new MpvWidget(this);
            }
            m_mpv2->setProperty("loop", true);
            m_mpv2->setProperty("panscan", 1);
            m_mpv2->setGeometry(geometry());
            m_mpv2->setProperty("volume", 0);
            m_mpv2->show();
        }
        if (m_mpv2) {
            m_mpv2->command(QStringList() << "loadfile" << path);
//            m_mpv2->setProperty("pause", true);
        }
    }

    refreashLayout();
}

void Wallpaper::setVolume(const qint32 volume)
{
    if (m_mpv) {
        m_mpv->setProperty("volume", volume);
    }
    if (m_mpv2) {
        m_mpv2->setProperty("volume", 0);
    }

}

void Wallpaper::clear()
{
    stop();
    hide();
}

void Wallpaper::play()
{
    if (!m_webView && m_mpv) {
        m_mpv->show();
        m_mpv->setProperty("pause", false);
        dApp->m_currentIsPlay = true;
    }
    if (m_mpv2) {
        m_mpv2->show();
        m_mpv2->setProperty("pause", false);
    }
}

void Wallpaper::pause()
{
    dApp->m_currentIsPlay = false;
    if (m_mpv) {
        m_mpv->setProperty("pause", true);
    }
    if (m_mpv2) {
        m_mpv2->setProperty("pause", true);
    }
}

void Wallpaper::stop()
{
    if (m_mpv) {
        m_mpv->setProperty("stop", true);
    }
    if (m_mpv2) {
        m_mpv2->setProperty("stop", true);
    }
}

void Wallpaper::slotrefreshPix(const QPixmap &pix)
{
    if (m_label2) {
        m_label2->setPixmap(pix);
    }
}

void Wallpaper::slotsetScreenMode(const QString &mode)
{
    if (mode == tr("Copy")) {
        if (dApp->m_cuurentMode != IdCopyScreen) {
            dApp->m_cuurentMode = IdCopyScreen;
            changeScreenMode(dApp->m_cuurentMode);
        }
    } else if (mode == tr("Extend")) {
        if (dApp->m_cuurentMode != IdlayoutScreen) {
            dApp->m_cuurentMode = IdlayoutScreen;
            changeScreenMode(dApp->m_cuurentMode);
        }
    } else if (mode == tr("Manual")) {
        if (dApp->m_cuurentMode != IdManualSet) {
            dApp->m_cuurentMode = IdManualSet;
            changeScreenMode(dApp->m_cuurentMode);
        }
    }


}

#include <QWindow>
#ifdef Q_OS_LINUX
#include <X11/extensions/shape.h>
#include <X11/extensions/Xrender.h>
#endif

#ifdef Q_OS_WINDOWS
#include <Windows.h>
#include <QDesktopWidget>
#include "basetsd.h"
#include "WinUser.h"
WId viewId;

LRESULT CALLBACK HookShoot(_In_ int nCode, _In_ WPARAM wParam,LPARAM lParam){
    if(wParam == WM_MOUSEMOVE || wParam == WM_NCMOUSEMOVE){
         MOUSEHOOKSTRUCT * mshook = (MOUSEHOOKSTRUCT *)lParam;
         PostMessage((HWND)viewId,WM_MOUSEMOVE,0,MAKELPARAM(mshook->pt.x,mshook->pt.y));
    };
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND _WORKERW = nullptr;

inline BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
{
    HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
    if (defview != nullptr)
    {
        _WORKERW = FindWindowEx(0, tophandle, L"WorkerW", 0);
    }
    return true;
}


BOOL sws_WindowHelpers_EnsureWallpaperHWND()
{
    // See: https://github.com/valinet/ExplorerPatcher/issues/525
    HWND progman = GetShellWindow();
    if (progman)
    {
        int res0 = -1 , res1 = -1, res2 = -1, res3 = -1 ;
        // Call CDesktopBrowser::_IsDesktopWallpaperInitialized
        SendMessageTimeoutW(progman, 0x052C, 0xA, 0, SMTO_NORMAL, 1000, (PDWORD_PTR)&res0);
        if (FAILED(res0))
        {
            return FALSE;
        }
        // Prepare to generate wallpaper window
        SendMessageTimeoutW(progman, 0x052C, 0xD, 0, SMTO_NORMAL, 1000, (PDWORD_PTR)&res1);
        SendMessageTimeoutW(progman, 0x052C, 0XD, 1, SMTO_NORMAL, 1000, (PDWORD_PTR)&res2);
        // "Animate desktop", which will make sure the wallpaper window is there
        SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, (PDWORD_PTR)&res3);  // 0 参数是必须的，对于早期系统(win7) 0xD 参数会导致处理失败。
        //printf("[sws] Wallpaper results: %d %d %d\n", res1, res2, res3);
        return !res1 && !res2 && !res3;
    }
    return FALSE;
}

HWND GetWorkerDesktop(){
    int result;
    HWND windowHandle = FindWindow(L"Progman", nullptr);
    //使用 0x3e8 命令分割出两个 WorkerW
    SendMessageTimeout(windowHandle, 0x052c, 0 ,0, SMTO_NORMAL, 0x3e8,(PDWORD_PTR)&result);
    //遍历窗体获得窗口句柄
    EnumWindows(EnumWindowsProc,(LPARAM)nullptr);
    // 显示WorkerW
    ShowWindow(_WORKERW,SW_HIDE);
    return windowHandle;

    //背景窗体没有窗体名，但是知道它的类名是workerW，且有父窗体Program Maneger，所以只要
    //遍历所有workW类型的窗体，逐一比较它的父窗体是不是Program Manager就可以找到背景窗体
//    HWND hwnd = FindWindowA("progman", "Program Manager");
//    HWND worker = NULL;
//    do {
//        worker = FindWindowExA(NULL, worker, "workerW", NULL);
//        if (worker != NULL) {
//            char buff[200] = {0};
//            int ret = GetClassNameA(worker, (PCHAR) buff, sizeof(buff) * 2);
//            if (ret == 0) {
//                return NULL;
//            }
//        }
//        if (GetParent(worker) == hwnd) {
//            return worker;//返回结果
//        }
//    } while (worker != NULL);
//    //没有找到
//    //发送消息生成一个WorkerW窗体
//    SendMessage(hwnd,0x052C,0,0);
//    //重复上面步骤
//    do {
//        worker = FindWindowExA(NULL, worker, "workerW", NULL);
//        if (worker != NULL) {
//            char buff[200] = {0};
//            int ret = GetClassNameA(worker, (PCHAR) buff, sizeof(buff) * 2);
//            if (ret == 0) {
//                return NULL;
//            }
//        }
//        if (GetParent(worker) == hwnd) {
//            return worker;//返回结果
//        }
//    } while (worker != NULL);
//    return NULL;

}
#endif

#define ATOM(a) XInternAtom(static_cast<Display *>(dApp->getDisplay()), #a, False)
void Wallpaper::registerDesktop()
{
#ifdef Q_OS_LINUX
    // if(dApp->m_screenDesktopWid.size() > 0)
    // {
    //     int i = dApp->m_screenDesktopWid.size();
    //     QWindow::fromWinId(winId())->setParent( QWindow::fromWinId(dApp->m_screenDesktopWid.at(0)));
    // }
    if(QGuiApplication::platformName() == "xcb") {

        Display * display = display = XOpenDisplay(NULL);
        Atom net_wm_window_type_desktop = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
        XChangeProperty(display, winId(), XInternAtom(display, "_NET_WM_WINDOW_TYPE", False),
                        XA_ATOM, 32, PropModeReplace, (unsigned char *) &net_wm_window_type_desktop, 1);

        XCloseDisplay(display);


    } else if(QGuiApplication::platformName() == "wayland") {
        // 不是X11环境，不执行相应代码
    }


    QTimer::singleShot(1, this, [ = ] {
        show();
        qDebug()<<this->size();
        lower();
    });
    if (!dApp->m_screenWid.contains(winId())) {
        dApp->m_screenWid.push_back(winId());
        QWindow *window = QWindow::fromWinId(winId());
        if(window)
        {
            window->setOpacity(dApp->m_moreData.m_WallpaperTransparency);
        }
    }
#else

//    HWND desktopHandle = FindWindow(L"Progman", nullptr);
//    HWND workerHandle = FindWindowEx(desktopHandle, nullptr, L"WorkerW", nullptr);
//    SetParent((HWND)winId(), workerHandle);

    bool bb = sws_WindowHelpers_EnsureWallpaperHWND();
    qDebug()<< bb;

    QTimer::singleShot(200,[=]
    {
        workerW =  GetWorkerDesktop();


        viewId = this->winId();
        // 返回workerW窗口句柄

        //设置窗口样式
        GetWindowLongA((HWND)viewId, GWL_STYLE);
        //设置壁纸窗体的父窗体
        SetParent((HWND)viewId,workerW);
        SetWindowPos((HWND)viewId,HWND_TOP,0,0,0,0,WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_RIGHTSCROLLBAR|WS_EX_NOACTIVATE);
        // 设置全局鼠标事件钩子
        //hook = SetWindowsHookEx(WH_MOUSE_LL,HookShoot,GetModuleHandle(NULL),0);

    });

#endif

//    Atom xa = 1247;
//    if (xa != None) {
//        long prop = 0;

//        XChangeProperty(static_cast<Display *>(dApp->getDisplay()), winId(), xa, XA_CARDINAL, 32,
//                        PropModeAppend, (unsigned char *) &prop, 1);
//    }

//    xa = 355;
//    if (xa != None) {
//        Atom xa_prop = 357;

//        XChangeProperty(static_cast<Display *>(dApp->getDisplay()), winId(), xa, XA_ATOM, 32,
//                        PropModeAppend, (unsigned char *) &xa_prop, 1);
//    }
//    QWindow *window = QWindow::fromWinId(winId());
//    if (window) {
//        window->setOpacity(0.99);
//    }

//    if (1) {
//        xa = ATOM(_WIN_LAYER);
//        if (xa != None) {
//            long prop = 0;

//            XChangeProperty(static_cast<Display *>(dApp->getDisplay()), winId(), xa, XA_CARDINAL, 32,
//                            PropModeAppend, (unsigned char *) &prop, 1);
//        }

//        xa = ATOM(_NET_WM_STATE);
//        if (xa != None) {
//            Atom xa_prop = ATOM(_NET_WM_STATE_BELOW);

//            XChangeProperty(static_cast<Display *>(dApp->getDisplay()), winId(), xa, XA_ATOM, 32,
//                            PropModeAppend, (unsigned char *) &xa_prop, 1);
//        }
//    }

//    //事件穿透
//    Region region;

//    region = XCreateRegion();
//    if (region) {
//        XShapeCombineRegion(static_cast<Display *>(dApp->getDisplay()), winId(), ShapeInput, 0, 0, region, ShapeSet);
//        XDestroyRegion(region);
//    }

}

bool Wallpaper::event(QEvent *event)
{
    if(event->type() == QEvent::Hide)
    {
        this->setVisible(true);
        this->activateWindow();
        this->raise();

        slotActiveWallpaper(dApp->m_moreData.isTop);
    }

    if (event->type() == QEvent::ActivationChange) {
        slotActiveWallpaper(dApp->m_moreData.isTop);
        QTimer::singleShot(1500,[=] {
            slotActiveWallpaper(dApp->m_moreData.isTop);
        });
    }


    return  QWidget::event(event);
}

void Wallpaper::onSysLockState(QString, QVariantMap key2value, QStringList)
{
    if (key2value.value("Locked").value<bool>()) {
        dApp->m_isNoMpvPause = false;
        pause();
    } else {
        play();
        dApp->m_isNoMpvPause = true;
    }
}

void Wallpaper::slotSetMpvValue(const QString &key, const QString &value)
{
    if (m_mpv) {
        m_mpv->setProperty(key, value);
    }
    if (m_mpv2) {
        m_mpv2->setProperty(key, value);
    }
}

void Wallpaper::slotSetTransparency(const int value)
{
    return;
    QWindow *win = QWindow::fromWinId(winId());
    double dvalue = (double)value;
    double dvalueOpacity = value / 100.0;
    win->setOpacity(dvalueOpacity);
}

void Wallpaper::updateGeometry()
{
    QTimer::singleShot(100, [ = ] {
        dApp->m_currentScreenNum = QGuiApplication::screens().size();

        QRect rec;
        QSize size1(0, 0);
        // 获取主屏幕
        QScreen *primaryScreen = QGuiApplication::primaryScreen();
        rec = primaryScreen->geometry();
        QRect rec2 = primaryScreen->geometry();
        QRect deskRect = primaryScreen->availableGeometry();
        rec = rec2;
        if (dApp->m_cuurentMode == IdCopyScreen)
        {
            rec = QRect(0, 0, rec.width(), rec.height());
            size1.setWidth(rec.width());
            size1.setHeight(rec.height());
            int twidth = 0;
            int theight = 0;
            for (auto screen : qApp->screens()) {
                twidth += screen->geometry().width();
                if (screen->geometry().height() > theight) {
                    theight = screen->geometry().height();
                }
            }

            this->setGeometry(QRect(0, 0, twidth, theight));
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
            this->hide();
            this->show();
            this->raise();
#endif
            QScreen * priScreen = nullptr;
            qDebug()<<this->size();

            QList<QScreen*> screens = QApplication::screens();

            if (screens.size() >= 2) {
                QScreen *leftScreen = nullptr;
                QScreen *rightScreen = nullptr;

                for (QScreen *screen : screens) {
                    QRect geometry = screen->geometry();
                    int xPos = geometry.x();

                    if (leftScreen == nullptr || xPos < leftScreen->geometry().x()) {
                        leftScreen = screen;
                    }

                    if (rightScreen == nullptr || xPos > rightScreen->geometry().x()) {
                        rightScreen = screen;
                    }
                }
                priScreen = leftScreen;
                qDebug()<<"left :" <<priScreen->name();
            }
            else if(screens.size() >= 1)
            {
                priScreen = screens.at(0);
                qDebug()<<"left :" <<priScreen->name();
            }
            int i = 1;
            int iX =0;

            for (auto screen : qApp->screens()) {
                dApp->m_currentScreenNum = QGuiApplication::screens().size();
                qDebug()<<"screen->name() "<<screen->name() ;
                if(screen->name() == priScreen->name() && m_mpv)
                {
                    m_mpv->setGeometry(screen->geometry());
                    m_mpv->setMinimumWidth(screen->geometry().width());
                    iX = screen->geometry().width();
                    qDebug()<<"Main : "<< screen->geometry();
                    i++;
                    continue;
                }
                if (screen->name() == priScreen->name()&& m_webView) {
                    m_webView->setGeometry(screen->geometry());
                    m_webView->setMinimumWidth(screen->geometry().width());
                    iX = screen->geometry().width();
                    i++;
                    continue;
                }
            }

            for (auto screen : qApp->screens()) {
                dApp->m_currentScreenNum = QGuiApplication::screens().size();
                qDebug()<<"screen->name() "<<screen->name() << screen->geometry().y() ;
                if(screen->name() != priScreen->name() && m_mpv2){
                    m_mpv2->setGeometry(iX,screen->geometry().y(),screen->geometry().width(),screen->geometry().height());
                    i++;
                     qDebug()<<"Main2 : "<< screen->geometry();
                    continue;
                }
                if (screen->name() != priScreen->name() && m_webView2) {
                    m_webView2->setGeometry(iX,0,screen->geometry().width(),screen->geometry().height());
                    i++;
                    continue;
                }
            }


        } else if (dApp->m_cuurentMode == IdlayoutScreen)
        {
            rec = QRect(0, 0, rec.width() * QGuiApplication::screens().size(), rec.height());
            size1.setWidth(rec.width());
            size1.setHeight(rec.height());
            this->setGeometry(rec);

            if (m_mpv) {
                m_mpv->setGeometry(rec);
            }
            if (m_mpv2) {
                this->layout()->removeWidget(m_mpv2);
                m_mpv2->deleteLater();
                m_mpv2 = nullptr;
            }
            if (m_webView) {
                m_webView->setGeometry(rec);
            }
            if (m_webView2) {
                this->layout()->removeWidget(m_webView2);
                m_webView2->deleteLater();
                m_webView2 = nullptr;
            }
        } else  if (dApp->m_cuurentMode == IdManualSet)
        {
            rec = dApp->m_manual;
            size1.setWidth(dApp->m_manual.width());
            size1.setHeight(dApp->m_manual.height());
            this->setGeometry(rec);
            if (m_mpv) {
                m_mpv->setGeometry(rec);
            }
            if (m_mpv2) {
                this->layout()->removeWidget(m_mpv2);
                m_mpv2->deleteLater();
                m_mpv2 = nullptr;
            }
            if (m_webView) {
                m_webView->setGeometry(rec);
            }
            if (m_webView2) {
                this->layout()->removeWidget(m_webView2);
                m_webView2->deleteLater();
                m_webView2 = nullptr;
            }
        }
        setIconVisble(dApp->m_moreData.isShowDesktopIcon);
        if (m_iconView && m_iconView->isVisible())
        {
            if (m_mpv) {
                m_iconView->setGeometry(deskRect);
                m_iconView->setFixedSize(deskRect.width(), deskRect.height());
                m_iconView->setParent(m_mpv);
            } else if (m_webView) {
                m_iconView->setGeometry(deskRect);
                m_iconView->setFixedSize(deskRect.width(), deskRect.height());
                m_iconView->setParent(m_webView);
            }
            m_iconView->show();
            m_iconView->move(deskRect.x(), deskRect.y());
        }
    });
}

void Wallpaper::slotMouseEvent()
{
    if (m_webView ) {
        QPoint pos = QCursor::pos();
        if (m_currentPos != pos) {
            m_currentPos = pos;

            for (QObject *obj : m_webView->children()) {
                QWidget *wgt = qobject_cast<QWidget *>(obj);
                if (wgt) {
                    LeftMouseMove(wgt, pos);
                }
            }
//            if (m_webView2) {
//                for (QObject *obj : m_webView2->children()) {
//                    QWidget *wgt = qobject_cast<QWidget *>(obj);
//                    if (wgt) {
//                        LeftMouseClick(wgt, pos);
//                    }
//                }
//            }
        }

    }
}

void Wallpaper::slotMouseClick(const int &index)
{
    if (m_webView) {
        QPoint pos = QCursor::pos();
//        if (m_currentPos != pos) {
        m_currentPos = pos;
//        QRect rec2 = qApp->desktop()->screenGeometry();
//        if (pos.x() > rec2.width()) {
//            pos = QPoint(pos.x() - rec2.width(), pos.y());
//        }
        for (QObject *obj : m_webView->children()) {
            QWidget *wgt = qobject_cast<QWidget *>(obj);
            if (wgt) {
                if (index == 0) {
                    LeftMousePress(wgt, pos);
                } else if (index == 1) {
                    LeftMouseRelease(wgt, pos);
                }

            }
        }
//            if (m_webView2) {
//                for (QObject *obj : m_webView2->children()) {
//                    QWidget *wgt = qobject_cast<QWidget *>(obj);
//                    if (wgt) {
//                        LeftMouseClick(wgt, pos);
//                    }
//                }
//            }
    }

}

void Wallpaper::slotActiveWallpaper(bool bRet)
{
    qDebug()<< "slotActiveWallpaper:" << bRet;
    if(!bRet)
    {
        for (auto wid : dApp->m_screenWid) {
            if(wid == this->winId())
            {
                this->raise();
                this->lower();
                QThread::msleep(100);
            }
            else
            {
                QWindow *window = QWindow::fromWinId(wid);
                if (window) {
                    window->raise();
                }
            }
        }
        for(auto wid : dApp->m_screenDesktopWid)
        {
            if(wid == this->winId())
            {
                this->lower();
            }
            else
            {
                QWindow *window = QWindow::fromWinId(wid);
                if (window) {
                    QThread::msleep(10);
                    window->raise();
                }
            }
        }
    }
    else
    {
        for (auto wid : dApp->m_screenWid) {
            if(wid == this->winId())
            {
                this->raise();
            }
            else
            {
                QWindow *window = QWindow::fromWinId(wid);
                if (window) {
                    window->lower();
                }
            }
        }
        for(auto wid : dApp->m_screenDesktopWid)
        {
            if(wid == this->winId())
            {
                this->raise();
            }
            else
            {
                QWindow *window = QWindow::fromWinId(wid);
                if (window) {
                    window->lower();
                }
            }
        }
    }
}

void Wallpaper::slotWallpaperEventChanged(bool bRet)
{
#ifdef Q_OS_LINUX
    Display * display = XOpenDisplay(NULL);
    Atom xa = 1247;
    if (xa != None) {
        long prop = 0;

        XChangeProperty(display, winId(), xa, XA_CARDINAL, 32,
                        PropModeAppend, (unsigned char *) &prop, 1);
    }

    xa = 355;
    if (xa != None) {
        Atom xa_prop = 357;

        XChangeProperty(display, winId(), xa, XA_ATOM, 32,
                        PropModeAppend, (unsigned char *) &xa_prop, 1);
    }
    QWindow *window = QWindow::fromWinId(winId());
    if (window) {
        window->setOpacity(0.99);
    }

    if (1) {
        xa = ATOM(_WIN_LAYER);
        if (xa != None) {
            long prop = 0;

            XChangeProperty(display, winId(), xa, XA_CARDINAL, 32,
                            PropModeAppend, (unsigned char *) &prop, 1);
        }

        xa = ATOM(_NET_WM_STATE);
        if (xa != None) {
            Atom xa_prop = ATOM(_NET_WM_STATE_BELOW);

            XChangeProperty(display, winId(), xa, XA_ATOM, 32,
                            PropModeAppend, (unsigned char *) &xa_prop, 1);
        }
    }

    //是否事件穿透
    Region region;

    region = XCreateRegion();
    if (region && bRet) {
        XShapeCombineRegion(display, winId(), ShapeInput, 0, 0, region, ShapeSet);
    }
    else
    {
        XShapeCombineMask(display, winId(), ShapeInput, 0, 0, None, ShapeSet);
    }
    dApp->changeMeOpacity(dApp->m_moreData.m_WallpaperTransparency);

    XCloseDisplay(display);
#else
    Q_UNUSED(bRet);
#endif
}

void Wallpaper::LeftMouseMove(QWidget *eventsReciverWidget, QPoint clickPos)
{
    QMouseEvent *press = new QMouseEvent(QEvent::MouseMove,
                                         clickPos,
                                         Qt::LeftButton,
                                         Qt::MouseButton::NoButton,
                                         Qt::NoModifier);
    QCoreApplication::postEvent(eventsReciverWidget, press);
}

void Wallpaper::LeftMouseRelease(QWidget *eventsReciverWidget, QPoint clickPos)
{
    QMouseEvent *release = new QMouseEvent(QEvent::MouseButtonRelease,
                                           clickPos,
                                           Qt::LeftButton,
                                           Qt::MouseButton::LeftButton,
                                           Qt::NoModifier);
    QCoreApplication::postEvent(eventsReciverWidget, release);
}


void Wallpaper::LeftMousePress(QWidget *eventsReciverWidget, QPoint clickPos)
{
    QMouseEvent *press = new QMouseEvent(QEvent::MouseButtonPress,
                                         clickPos,
                                         Qt::LeftButton,
                                         Qt::MouseButton::LeftButton,
                                         Qt::NoModifier);
    QCoreApplication::postEvent(eventsReciverWidget, press);

}

void Wallpaper::setIconVisble(bool visble)
{
    if (m_iconView) {
        m_iconView->setVisible(visble);
    }
}

void Wallpaper::refreashLayout()
{
    if(m_mpv)
    {
        layout()->removeWidget(m_mpv);
    }
    if(m_webView)
    {
        layout()->removeWidget(m_webView);
    }
    if(m_mpv2)
    {
        layout()->removeWidget(m_mpv2);
    }
    if(m_webView2)
    {
        layout()->removeWidget(m_webView2);
    }
    if(m_mpv)
    {
        layout()->addWidget(m_mpv);
    }
    if(m_webView)
    {
        layout()->addWidget(m_webView);
    }
    if(m_mpv2)
    {
        layout()->addWidget(m_mpv2);
    }
    if(m_webView2)
    {
        layout()->addWidget(m_webView2);
    }
    //暂时调用两次,为保证切换顺利
    QTimer::singleShot(10, [ = ] {
        updateGeometry();
    });
    updateGeometry();
}
