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

#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>
#include <malloc.h>

#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QtX11Extras/QX11Info>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QDesktopWidget>
#include <QDebug>
#include <QLabel>
#include <QDBusConnection>
#include <QWebEngineView>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QEvent>
#include <QMouseEvent>

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
#include <iostream>
#include <QStandardPaths>

using namespace std;

#include "application.h"

Wallpaper::Wallpaper(QString path, int currentScreen, QWidget *parent)
    : QWidget(parent)
    , m_currentPath(path)
    , m_currentScreen(currentScreen)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);

    registerDesktop();

    connect(dApp, &Application::refreshPix, this, &Wallpaper::slotrefreshPix);
    connect(dApp, &Application::setScreenMode, this, &Wallpaper::slotsetScreenMode);
    connect(qApp->desktop(), &QDesktopWidget::resized, this, &Wallpaper::updateGeometry);
    connect(dApp, &Application::setPlayPath, this, &Wallpaper::setFile);
    connect(dApp, &Application::setMpvPlay, this, &Wallpaper::play);
    connect(dApp, &Application::setMpvpause, this, &Wallpaper::pause);
    connect(dApp, &Application::setMpvstop, this, &Wallpaper::stop);
    connect(dApp, &Application::setMpvVolume, this, &Wallpaper::setVolume);
    connect(dApp, &Application::setScreen, this, &Wallpaper::setScreen);
    connect(dApp, &Application::sigupdateGeometry, this, &Wallpaper::updateGeometry);
    connect(dApp, &Application::setMpvValue, this, &Wallpaper::slotSetMpvValue);

    connect(dApp, &Application::sigSetTransparency, this, &Wallpaper::slotSetTransparency);

    QDesktopWidget *desktopwidget = QApplication::desktop();
    connect(desktopwidget, &QDesktopWidget::resized, this, [ = ] {
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
    m_mouseWebEventTimer->start(30);

    QString paths = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    m_iconView = new IconView(0, paths, this);
    connect(m_iconView, &IconView::sigMouseClick, this, &Wallpaper::slotMouseClick);
    m_iconView->move(0, 0);

    QTimer::singleShot(1, this, &Wallpaper::updateGeometry);
    QTimer::singleShot(1000, this, [ = ] {
        int index = 0;
        int index1 = 0;
        for (const QString &arg : qApp->arguments())
        {
            if (index != 0) {
                if (QFileInfo(arg).isFile()) {
                    setFile(arg);
                    play();
                    index1++;
                }
            }
            index++;
        }
        if (index1 == 0)
        {
            QString playPath = "/usr/share/fantascene-dynamic-wallpaper/normal/normal.mp4";

            m_currentPath = m_currentPath.replace("file://", "");
            if (!m_currentPath.isEmpty()) {
                if (QFileInfo(m_currentPath).isFile()) {
                    playPath = m_currentPath;
                } else if (path.contains("www") || path.contains("http//") || path.contains("https//")) {
                    playPath = m_currentPath;
                }
            }
            setFile(playPath);
            play();
            m_currentPath = QFileInfo(m_currentPath).filePath();
            Q_EMIT dApp->pathChanged(m_currentPath);
        }
//        QTimer::singleShot(100, [ = ] {
//            updateGeometry();
//        });
    });

    setVolume(0);





}

void Wallpaper::changeScreenMode(ScreenMode mode)
{
    switch (mode) {
    case IdCopyScreen: {
        if (qApp->desktop()->screenCount() > 1) {

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
                layout()->addWidget(m_webView2);
            } else if (nullptr == m_mpv2) {
                m_mpv2 = new MpvWidget(this);
                m_mpv2->setVisible(true);
                m_mpv2->setProperty("loop", true);
                m_mpv2->setProperty("panscan", 1);
                m_mpv2->command(QStringList() << "loadfile" << path);
                m_mpv2->setProperty("pause", true);
                m_mpv2->setProperty("volume", 0);
                layout()->addWidget(m_mpv2);

            }

        }
        break;
    }
    case IdlayoutScreen: {
        if (!m_webView2) {
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
        if (!m_webView2) {
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

    malloc_trim(0);
//    de->setParent(this);
    if (path.contains("html") || path.contains("www") || path.contains("http//") || path.contains("https//")) {
        if (m_mpv2) {
            layout()->removeWidget(m_mpv2);
            m_mpv2->deleteLater();
            m_mpv2 = nullptr;
        }
        if (m_mpv) {
            layout()->removeWidget(m_mpv);
            m_mpv->deleteLater();
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
        layout()->addWidget(m_webView);
        pause();
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

                layout()->addWidget(m_webView2);
            }
        }

    }  else {
        if (m_webView) {
            layout()->removeWidget(m_webView);
            delete m_webView;
            m_webView = nullptr;
        }
        if (m_webView2) {
            layout()->removeWidget(m_webView2);
            delete m_webView2;
            m_webView2 = nullptr;
        }
        if (!m_mpv) {
            m_mpv = new MpvWidget(this);
            m_mpv->setGeometry(geometry());

            m_mpv->setProperty("loop", true);
            m_mpv->setProperty("panscan", 1);
            m_mpv->setGeometry(geometry());
            m_mpv->show();
            layout()->addWidget(m_mpv);


            if (qApp->screens().count() > 1 && IdCopyScreen == dApp->m_cuurentMode) {
                if (!m_mpv2) {
                    m_mpv2 = new MpvWidget(this);
                }
                m_mpv2->setProperty("loop", true);
                m_mpv2->setProperty("panscan", 1);
                m_mpv2->setGeometry(geometry());
                m_mpv2->setProperty("volume", 0);
                m_mpv2->show();
                layout()->addWidget(m_mpv2);
            }
        }

        m_mpv->command(QStringList() << "loadfile" << path);
        m_mpv->setProperty("pause", true);
        if (m_mpv2) {
            m_mpv2->command(QStringList() << "loadfile" << path);
            m_mpv2->setProperty("pause", true);
        }


    }
    //        //发送读取配置文件
    Q_EMIT dApp->sigReadPlayerConfig();

    //暂时调用两次,为保证切换顺利
    QTimer::singleShot(10, [ = ] {
        updateGeometry();
    });
    updateGeometry();
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
#define ATOM(a) XInternAtom(QX11Info::display(), #a, False)
void Wallpaper::registerDesktop()
{
    xcb_ewmh_connection_t m_ewmh_connection;
    xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(QX11Info::connection(), &m_ewmh_connection);
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);

    xcb_atom_t atoms[1];
    atoms[0] = m_ewmh_connection._NET_WM_WINDOW_TYPE_DESKTOP;
    xcb_ewmh_set_wm_window_type(&m_ewmh_connection, winId(), 1, atoms);

    QTimer::singleShot(1, this, [ = ] {
        show();
        lower();
    });
    if (!dApp->m_screenWid.contains(winId())) {
        dApp->m_screenWid.push_back(winId());
    }

//    Atom xa = 1247;
//    if (xa != None) {
//        long prop = 0;

//        XChangeProperty(QX11Info::display(), winId(), xa, XA_CARDINAL, 32,
//                        PropModeAppend, (unsigned char *) &prop, 1);
//    }

//    xa = 355;
//    if (xa != None) {
//        Atom xa_prop = 357;

//        XChangeProperty(QX11Info::display(), winId(), xa, XA_ATOM, 32,
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

//            XChangeProperty(QX11Info::display(), winId(), xa, XA_CARDINAL, 32,
//                            PropModeAppend, (unsigned char *) &prop, 1);
//        }

//        xa = ATOM(_NET_WM_STATE);
//        if (xa != None) {
//            Atom xa_prop = ATOM(_NET_WM_STATE_BELOW);

//            XChangeProperty(QX11Info::display(), winId(), xa, XA_ATOM, 32,
//                            PropModeAppend, (unsigned char *) &xa_prop, 1);
//        }
//    }

//    //事件穿透
//    Region region;

//    region = XCreateRegion();
//    if (region) {
//        XShapeCombineRegion(QX11Info::display(), winId(), ShapeInput, 0, 0, region, ShapeSet);
//        XDestroyRegion(region);
//    }

}

bool Wallpaper::event(QEvent *event)
{
    if (event->type() == QEvent::WindowActivate) {
        for (auto wid : dApp->m_screenWid) {
            QWindow *window = QWindow::fromWinId(wid);
            if (window) {
                window->raise();
            }
        }
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
        dApp->m_currentScreenNum = dApp->desktop()->screenCount();
        QRect rec;
        QSize size1(0, 0);
        rec = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
        QRect rec2 = qApp->desktop()->screenGeometry();
        QRect deskRect = qApp->desktop()->availableGeometry();
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

            int i = 1;
            int iX =0;
            for (auto screen : qApp->screens()) {
                dApp->m_currentScreenNum = dApp->desktop()->screenCount();
                if (i == 1 && m_mpv) {
                    m_mpv->setGeometry(screen->geometry());
                    m_mpv->setMinimumWidth(screen->geometry().width());
                    iX = screen->geometry().width();
                    i++;
                    continue;
                }
                if (i == 2 && m_mpv2) {
                    m_mpv2->setGeometry(iX,0,screen->geometry().width(),screen->geometry().height());
                    i++;
                    continue;
                }
                if (i == 1 && m_webView) {
                    m_webView->setGeometry(screen->geometry());
                    m_webView->setMinimumWidth(screen->geometry().width());
                    iX = screen->geometry().width();
                    i++;
                    continue;
                }
                if (i == 2 && m_webView2) {
                    m_webView2->setGeometry(iX,0,screen->geometry().width(),screen->geometry().height());
                    i++;
                    continue;
                }

            }

        } else if (dApp->m_cuurentMode == IdlayoutScreen)
        {
            rec = QRect(0, 0, rec.width() * dApp->desktop()->screenCount(), rec.height());
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
    if (m_webView && m_iconView->isVisible()) {
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
