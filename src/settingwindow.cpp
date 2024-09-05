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
#include "settingwindow.h"

#include "ui_settingwindow.h"
#include "setdesktop.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QSettings>
#include <QCoreApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QDesktopServices>
#include <QStringList>

#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QtConcurrent>
#include <QScreen>
#include "inimanager.h"

#include <QMutexLocker>

#include "listview/historywidget.h"
#include "listview/localwidget.h"
#include "listview/wallpaperengineplugin.h"
#include "help/helpdialog.h"
#include "download/downloadwidget.h"
#include "db/dbmanager.h"

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xcb/shape.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif
settingWindow::settingWindow(QWidget *parent, QMainWindow *mainWindow) :
    QWidget(parent),
    m_parentMainWindow(mainWindow),
    ui(new Ui::settingWindow)
{
    ui->setupUi(this);
    //读配置之前需要判断是否有可以支持的原生桌面
    dApp->CheckSystem();
    readSettings();

    ui->tansparency_slider->hide();
    ui->label_8->hide();
    /*qApp->*/installEventFilter(this);
    ui->pathEdit->installEventFilter(this);
    ui->pathEdit->setAcceptDrops(true);
    setAcceptDrops(true);

    bool isPath2 = dApp->m_isPath2 && QGuiApplication::screens().size() > 1;
    ui->pathBtn2->setVisible(isPath2);
    ui->pathEdit2->setVisible(isPath2);
    ui->pixThumbnail2->setVisible(isPath2);
    ui->setBtn2->setVisible(isPath2);
    ui->checkBox2Inde->setVisible(QGuiApplication::screens().size() > 1);


    m_traymenu = new QMenu();
    m_traymenu->setAttribute(Qt::WA_TranslucentBackground);

    QAction *exitAction = new QAction(m_traymenu);
    exitAction->setText(tr("Exit"));
    connect(exitAction, &QAction::triggered, this, &settingWindow::quitApp);
    connect(dApp, &Application::quitApp, this, &settingWindow::quitApp, Qt::DirectConnection);
    connect(dApp, &Application::sigWallpaperAction, this,[=]
    {
        if (m_parentMainWindow)
        {
            m_parentMainWindow->resize(500, 300);
            m_parentMainWindow->show();
            m_parentMainWindow->activateWindow();
        }
    });

    QAction *setMpvPlayAction = new QAction(m_traymenu);
    setMpvPlayAction->setText(tr("Play"));
    connect(setMpvPlayAction, &QAction::triggered, this, [ = ] {
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
    });

    QAction *setScreenshotAction = new QAction(m_traymenu);
    setScreenshotAction->setText(tr("Screenshot"));
    connect(setScreenshotAction, &QAction::triggered, this, [ = ] {
        Q_EMIT dApp->sigscreenshot();
    });
    QAction *setMpvpauseAction = new QAction(m_traymenu);
    setMpvpauseAction->setText(tr("Pause"));
    connect(setMpvpauseAction, &QAction::triggered, this, [ = ] {
        Q_EMIT dApp->setMpvpause();
        dApp->m_isNoMpvPause = false;
    });

    QAction *setHistoryAction = new QAction(m_traymenu);
    setHistoryAction->setText(tr("Wallpaper History"));
    connect(setHistoryAction, &QAction::triggered, this, &settingWindow::on_history_clicked);

    QAction *setMainViewAction = new QAction(m_traymenu);
    setMainViewAction->setText(tr("Main View"));
    connect(setMainViewAction, &QAction::triggered, this, [ = ] {
        if (m_parentMainWindow)
        {
            m_parentMainWindow->resize(500, 300);
            m_parentMainWindow->show();
            m_parentMainWindow->activateWindow();
        }
    });


    QAction *maingithub;
    QLocale locale = QLocale::system();
    QString language = locale.name();
    if (language == "zh_CN" || language == "zh_TW" || language == "zh_HK") {
        // 当前语言环境是中文
        maingithub = new QAction(m_traymenu);
        maingithub->setText(tr("Help"));
        connect(maingithub, &QAction::triggered, this, [ = ] {
            HelpDialog dialog;
            dialog.exec();
        });
    } else {
        // 当前语言环境不是中文
        maingithub = new QAction(m_traymenu);
        maingithub->setText(tr("github"));
        connect(maingithub, &QAction::triggered, this, [ = ] {
            QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/")));
        });
    }


    m_traymenu->addAction(setMainViewAction);
    m_traymenu->addAction(setHistoryAction);
    m_traymenu->addAction(setMpvPlayAction);
    m_traymenu->addAction(setMpvpauseAction);
    m_traymenu->addAction(setScreenshotAction);
    m_traymenu->addAction(maingithub);
    m_traymenu->addAction(exitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_traymenu);
    m_trayIcon->setToolTip(tr("Double click to open the configuration interface"));
    m_trayIcon->setIcon(QIcon(":/install/wallpaper.png"));
    m_trayIcon->show();

    //void activated(QSystemTrayIcon::ActivationReason reason);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [ = ](QSystemTrayIcon::ActivationReason reason) {
        if (QSystemTrayIcon::Trigger == reason) {
            this->show();
            if (m_parentMainWindow) {
                m_parentMainWindow->resize(500, 300);
                m_parentMainWindow->show();
                m_parentMainWindow->activateWindow();
            }
        }
    });

    connect(dApp, &Application::pathChanged, this, &settingWindow::pathChanged);
    connect(dApp, &Application::pathChanged2, this, &settingWindow::pathChanged2);
    connect(dApp, &Application::setWallPaper, this, &settingWindow::slotWallPaper);
    connect(dApp, &Application::setWallPaper2, this, &settingWindow::slotWallPaper2);

    connect(dApp, &Application::saveSetting, this, &settingWindow::saveSettings);

    connect(dApp, &Application::moreSettingSave, this, &settingWindow::slotMoreSettingSave);

    connect(dApp, &Application::sigActiveWindow, this, &settingWindow::activeWindow);
    connect(dApp, &Application::sigDesktopActive, this, [ = ] {
        this->show();
        if (m_parentMainWindow)
        {
            m_parentMainWindow->resize(500, 300);
            m_parentMainWindow->show();
            m_parentMainWindow->activateWindow();
        }
    });

    connect(dApp, &Application::sigReadPlayerConfig, this, [ = ] {
        if (m_voiceVolume >= 0 && m_voiceVolume < 100)
        {
            ui->Slider->setValue(m_voiceVolume);
            on_Slider_valueChanged(m_voiceVolume);
        }
    }, Qt::DirectConnection);

    ui->bugBtn->hide();
    ui->mainWeb->hide();
    ui->githubWeb->hide();
    ui->giteeWeb->hide();

    if (m_parentMainWindow) {
        m_aboutMenu = new QMenu();

        QAction *aboutMe = new QAction(m_aboutMenu);
        aboutMe->setText(tr("Latest version"));
        connect(aboutMe, &QAction::triggered, this, [ = ] {
            QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/releases")));
        });

        m_aboutMenu->addAction(aboutMe);

    }
#ifdef Q_OS_LINUX
    initAtom();
#endif
    initWallpaperWidget();
    executeSettings();

}
void settingWindow::pathChanged(const QString &path)
{
    dApp->m_currentPath = path;
    if (!dApp->m_currentPath.isEmpty()) {
        ui->pathEdit->setText(dApp->m_currentPath);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
    }
}

void settingWindow::pathChanged2(const QString &path)
{
    dApp->m_currentPath2 = path;
    if (!dApp->m_currentPath2.isEmpty()) {
        ui->pathEdit2->setText(dApp->m_currentPath2);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath2);
        if (!pix.isNull()) {
            ui->pixThumbnail2->setPixmap(pix);
        }
    }
}

settingWindow::~settingWindow()
{
    if (m_history) {
        m_history->deleteLater();
        m_history = nullptr;
    }
    delete ui;
}

void settingWindow::readSettings()
{
    dApp->m_currentPath = IniManager::instance()->value("WallPaper/CurrentPath").toString();
    dApp->m_isPath2 = IniManager::instance()->value("WallPaper/IsPath2").toBool();
    dApp->m_currentPath2 = IniManager::instance()->value("WallPaper/CurrentPath2").toString();
    m_crrenNumber = IniManager::instance()->value("WallPaper/ScrrenNumber").toInt(); //1-2
    m_isAutoStart = IniManager::instance()->value("WallPaper/isAutoStart").toInt();
    int widthPY = IniManager::instance()->value("WallPaper/widthPY").toInt();
    int heightPY = IniManager::instance()->value("WallPaper/heightPY").toInt();
    int width = IniManager::instance()->value("WallPaper/width").toInt();
    int height = IniManager::instance()->value("WallPaper/height").toInt();
    m_currentMode = IniManager::instance()->value("WallPaper/Mode").toString();
    m_voiceVolume = IniManager::instance()->value("WallPaper/voiceVolume").toInt();
    m_videoAspect = IniManager::instance()->value("WallPaper/videoAspect").toDouble();
    m_videoASpectStr = IniManager::instance()->value("WallPaper/videoAspectStr").toString();

    dApp->m_moreData.isAuto = IniManager::instance()->value("WallPaper/videoAutoMode").toInt();
    dApp->m_moreData.fps = IniManager::instance()->value("WallPaper/fps").toInt();
    dApp->m_moreData.hwdec = IniManager::instance()->value("WallPaper/hwdec").toString();
    dApp->m_moreData.vo = IniManager::instance()->value("WallPaper/vo").toString();
    dApp->m_wallpaperEnginePath = IniManager::instance()->value("WallPaper/wallpaperEnginePath").toString();
    if(IniManager::instance()->contains("WallPaper/isPlayList")){
        dApp->m_isPlayList = IniManager::instance()->value("WallPaper/isPlayList").toBool();
    }
    dApp->m_PlaylistTimer = IniManager::instance()->value("WallPaper/playlistTimer").toInt();
    if(IniManager::instance()->contains("WallPaper/desktopShow")){
        dApp->m_moreData.isShowDesktopIcon = IniManager::instance()->value("WallPaper/desktopShow").toBool();
    }
    if(IniManager::instance()->contains("WallPaper/topDefault")){
        dApp->m_moreData.isTop = IniManager::instance()->value("WallPaper/topDefault").toBool();
    }
    if(IniManager::instance()->contains("Wallpaper/EventPenetration"))
    {
        dApp->m_moreData.isEventPenetration = IniManager::instance()->value("Wallpaper/EventPenetration").toBool();
    }
    if(IniManager::instance()->contains("Wallpaper/DesktopFontColor"))
    {
        dApp->m_moreData.fontColor =IniManager::instance()->value("Wallpaper/DesktopFontColor").toString();
        dApp->setAppColor(dApp->m_moreData.fontColor);
    }


    dApp->setSpecialDesktop();

    dApp->setisPlayList(dApp->m_isPlayList);
    dApp->setPlayListTimer(dApp->m_PlaylistTimer);

    if (dApp->m_moreData.hwdec.isEmpty()) {
        dApp->m_moreData.hwdec = "gpu";
    }
    if(IniManager::instance()->contains("WallPaper/DesktopTransparency"))
    {
        dApp->m_moreData.m_DesktopTransparency = IniManager::instance()->value("WallPaper/DesktopTransparency").toDouble();
        dApp->changePidOpacity(dApp->m_moreData.m_DesktopTransparency);
    }
    if(IniManager::instance()->contains("WallPaper/WallpaperTransparency"))
    {
        dApp->m_moreData.m_WallpaperTransparency = IniManager::instance()->value("WallPaper/WallpaperTransparency").toDouble();
        dApp->changeMeOpacity(dApp->m_moreData.m_WallpaperTransparency);
    }

    ui->videoBLEdit->setText(QString::number(m_videoAspect));
    ui->videoBLCombox->setCurrentText(m_videoASpectStr);

    QStringList strList = DBManager::instance()->getAllPath();
    if(strList.count()>0)
    {
        for(QString strLocalPath: strList)
        {
            if (!dApp->m_allPath.contains(strLocalPath)) {
                dApp->m_allPath.push_back(strLocalPath);
                Q_EMIT dApp->addPaperView(strLocalPath);
            }
        }

        QStringList playList = DBManager::instance()->getAllPlayList();
        for(QString strPlaylistPath: playList)
        {
            if (!dApp->m_playlistPath.contains(strPlaylistPath))
            {
                dApp->m_playlistPath.push_back(strPlaylistPath);
                Q_EMIT dApp->addplaylist(strPlaylistPath);
            }
        }
    }
    else
    {
        //取值本地地址
        QString strLocalPath;
        int localIndex = 1;
        QList <WallpaperData> listData;
        do {
            QString key = "Movie/localPath" + QString::number(localIndex++);
            strLocalPath = IniManager::instance()->value(key).toString();

            if (nullptr != strLocalPath) {
                if (!dApp->m_allPath.contains(strLocalPath)) {
                    dApp->m_allPath.push_back(strLocalPath);
                    Q_EMIT dApp->addPaperView(strLocalPath);
                    WallpaperData data;
                    data.path = strLocalPath;
                    data.name = QFileInfo(strLocalPath).completeBaseName();
                    listData << data;
                }
            }
            IniManager::instance()->remove(key);
        } while (nullptr != strLocalPath);
        //加入数据库
        DBManager::instance()->addDatas(listData);
        //取值本地地址
        QString strPlaylistPath;
        int playlistIndex = 1;
        do {
            QString key = "Movie/playlistPath" + QString::number(playlistIndex++);
            strPlaylistPath = IniManager::instance()->value(key).toString();
            if (nullptr != strPlaylistPath) {
                if (!dApp->m_playlistPath.contains(strPlaylistPath)) {
                    dApp->m_playlistPath.push_back(strPlaylistPath);
                    Q_EMIT dApp->addplaylist(strPlaylistPath);
                }
            }
            IniManager::instance()->remove(key);
        } while (nullptr != strPlaylistPath);

        //加入数据库
        DBManager::instance()->addPlayList(dApp->m_playlistPath);
    }
    if(dApp->m_isPath2 && dApp->m_currentScreenNum > 1)
    {
        ui->checkBox2Inde->setChecked(true);
        ui->pathBtn2->setVisible(true);
        ui->pathEdit2->setVisible(true);
        ui->pixThumbnail2->setVisible(true);
        ui->setBtn2->setVisible(true);
    }
    else
    {
        ui->checkBox2Inde->setChecked(false);
        ui->pathBtn2->setVisible(false);
        ui->pathEdit2->setVisible(false);
        ui->pixThumbnail2->setVisible(false);
        ui->setBtn2->setVisible(false);
    }

    if(width <= 0)
    {
        width = 1;
    }
    if(height <= 0)
    {
        height = 1;
    }
    dApp->m_manual.setRect(widthPY, heightPY, width, height);
}

void settingWindow::executeSettings()
{

    if (!m_currentMode.isEmpty()) {
        ui->comboBox->setCurrentText(m_currentMode);
        setScreenMode(m_currentMode);
    }
    if (!dApp->m_currentPath.isEmpty()) {
        ui->pathEdit->setText(dApp->m_currentPath);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
    }
    if (!dApp->m_currentPath2.isEmpty()) {
        ui->pathEdit2->setText(dApp->m_currentPath2);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath2);
        if (!pix.isNull()) {
            ui->pixThumbnail2->setPixmap(pix);
        }
    }

    ui->widthPY->setText(QString::number(dApp->m_manual.x()));
    ui->heightPY->setText(QString::number(dApp->m_manual.y()));
    ui->width->setText(QString::number(dApp->m_manual.width()));
    ui->height->setText(QString::number(dApp->m_manual.height()));

    if (m_voiceVolume >= 0 && m_voiceVolume < 100) {
        ui->Slider->setValue(m_voiceVolume);
        on_Slider_valueChanged(m_voiceVolume);
    }
    if (!m_currentMode.isEmpty()) {
        ui->comboBox->setCurrentText(m_currentMode);
        setScreenMode(m_currentMode);
    }
    if (m_crrenNumber > 1) {
        //        ui->autoisMScreen->setCheckState(Qt::Checked);
    }
    if (m_isAutoStart > 0) {
        ui->autoStartBox->setCheckState(Qt::Checked);
    }

    on_videoBLCombox_activated(m_videoASpectStr);



    if (dApp->m_moreData.isAuto == 1) {

    }
    if (!dApp->m_moreData.hwdec.isEmpty()) {
        dApp->setMpvValue("hwdec", dApp->m_moreData.hwdec);
    }
    on_checkBox_stateChanged(dApp->m_moreData.isAuto);

    if (m_wallpaper) {
        m_wallpaper->setIconVisble(dApp->m_moreData.isShowDesktopIcon);
    }

}

void settingWindow::saveSettings()
{
    if(!m_timerSave){
        m_timerSave=new QTimer(this);
        connect(m_timerSave,&QTimer::timeout,this,&settingWindow::slotTimerSaveSettings);
    }
    m_timerSave->setSingleShot(true);
    m_timerSave->start(1000);
}

QString settingWindow::getCurrentPath()
{
    return dApp->m_currentPath;
}

int settingWindow::getCurrentNumber()
{
    return m_crrenNumber;
}

int settingWindow::isAutoStart()
{
    return m_isAutoStart;
}
#ifdef Q_OS_LINUX
QVector <WId> settingWindow::currentWorkWindow()
{
    QList<WId> currentApplicationList;
    const QWindowList &list = qApp->allWindows();
    currentApplicationList.reserve(list.size());
    for (auto window : list) {
        if (window->property("_q_foreignWinId").isValid()) continue;

        currentApplicationList.append(window->winId());
    }
    QVector<quint32> winList;
    winList = getCurrentWorkspaceWindows();

    for (WId wid : winList) {
        if (currentApplicationList.contains(wid)) {
            continue;
        }
        if (m_windowList.contains(wid)) {
            continue;
        }
        m_windowList.push_back(wid);
    }
    return m_windowList;
}
#endif

void settingWindow::setScreenMode(const QString &arg)
{
    Q_EMIT dApp->setScreenMode(arg);
}

void settingWindow::on_pathBtn_clicked()
{
    QString path ;
    if(dApp->m_isNoMpvPause)
    {
        Q_EMIT dApp->setMpvpause();
        dApp->m_isNoMpvPause = false;
        path = QFileDialog::getOpenFileUrl().toLocalFile();

        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
    }

    if (!path.isEmpty()) {
        ui->pathEdit->setText(path);
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
    }
}

void settingWindow::on_setBtn_clicked()
{
    if (ui->pathEdit->text() != nullptr) {
        dApp->m_currentPath = ui->pathEdit->text();
        dApp->m_currentPath = dApp->m_currentPath.replace("file://", "");
        Q_EMIT dApp->setPlayPath(ui->pathEdit->text());
        if(QGuiApplication::screens().size() > 1 && !dApp->m_isPath2)
        {
            Q_EMIT dApp->setPlayPath2(ui->pathEdit->text());
        }
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        saveSettings();
        Q_EMIT dApp->addPaperView(dApp->m_currentPath);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
        dApp->addLocalPaths(QStringList(dApp->m_currentPath));
    }
}

void settingWindow::on_cancelBtn_clicked()
{
    if (m_parentMainWindow) {
        m_parentMainWindow->close();
    }
}

void settingWindow::on_pauseBtn_clicked()
{
    Q_EMIT dApp->setMpvpause();
    dApp->m_isNoMpvPause = false;
}

void settingWindow::on_stopBtn_clicked()
{
    Q_EMIT dApp->setMpvstop();
    dApp->m_isNoMpvPause = false;
}

void settingWindow::on_Slider_valueChanged(int value)
{
    Q_EMIT dApp->setMpvVolume(value);
    m_voiceVolume = value;
    IniManager::instance()->setValue("WallPaper/voiceVolume", m_voiceVolume);
    saveSettings();
}

void settingWindow::on_startBtn_clicked()
{
    Q_EMIT dApp->setMpvPlay();
    dApp->m_isNoMpvPause = true;
}

void settingWindow::on_startScreen_clicked()
{
    if (index == 1) {
        index = 2;
    } else {
        index = 1;
    }
    Q_EMIT dApp->setScreen(index);
    on_setBtn_clicked();
    on_setBtn2_clicked();
    on_startBtn_clicked();

}

void settingWindow::on_autoStart_clicked()
{

    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/";
    QDir folder(path);
    if (!folder.exists()) {
        if (folder.mkpath(path)) {
            qDebug() << "Folder created successfully";
        } else {
            qDebug() << "Failed to create folder";
        }
    }
    if (!QFileInfo(path + "fantascene-dynamic-wallpaper.desktop").isFile())
        QProcess::execute("cp /usr/share/applications/fantascene-dynamic-wallpaper.desktop " + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/");
}

void settingWindow::on_noAutoStart_clicked()
{
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/fantascene-dynamic-wallpaper.desktop");
}

void settingWindow::on_autoStartBox_stateChanged(int arg1)
{
    if (arg1 == 0) {
        m_isAutoStart = 0;
        on_noAutoStart_clicked();
    } else {
        m_isAutoStart = 1;
        on_autoStart_clicked();
    }
    saveSettings();
}

void settingWindow::on_comboBox_activated(const QString &arg1)
{
    setScreenMode(arg1);
    saveSettings();
}

void settingWindow::on_setManual_clicked()
{
    int widthPY = ui->widthPY->text().toInt();
    int heightPY = ui->heightPY->text().toInt();
    int width = ui->width->text().toInt();
    int height = ui->height->text().toInt();
    if(width <= 0)
    {
        width = 1;
    }
    if(height <= 0)
    {
        height = 1;
    }
    dApp->m_manual.setRect(widthPY, heightPY, width, height);
    if (ui->comboBox->currentText() == tr("Manual")) {
        Q_EMIT dApp->sigupdateGeometry();
    }
    saveSettings();
}

void settingWindow::quitApp()
{


    //#ifdef QT_NO_DEBUG
    //    QProcess::execute("killall dde-desktop");
    //    if (0 != dApp->m_processId) {
    //        QProcess::execute("kill " + QString::number(dApp->m_processId));
    //    }
    //    QThread *th = QThread::create([ = ]() {
    //        QProcess::execute("dde-desktop");
    //    });
    //    th->start();
    //    saveSettings();
    //#else
    slotTimerSaveSettings();
    //#endif
    //dbus关闭壁纸透明

    //    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground true");
    m_stopx11Thread = true;
    if (m_x11thread) {
        m_x11thread->wait();
        m_x11thread->quit();
    }

    if(!dApp->m_moreData.isShowDesktopIcon && !dApp->m_moreData.isTop )
    {
        //kylin os quit!
        if(dApp->m_isUKUI)
        {
           QString command1 = "gsettings set org.mate.background picture-filename ''";
           QProcess process1;
           process1.start(command1);
           process1.waitForFinished(-1);  // 等待进程执行完成
        }
        else if(dApp->m_isGXDE)
        {
            QDBusInterface iface("com.deepin.dde.desktop",
                                 "/com/deepin/dde/desktop",
                                 "com.deepin.dde.desktop",
                                 QDBusConnection::sessionBus());
            iface.asyncCall("EnableBackground", true);
        }
        else if(dApp->m_isLingMo)
        {
            QDBusInterface iface("com.lingmo.Settings",
                                 "/Theme",
                                 "com.lingmo.Theme",
                                 QDBusConnection::sessionBus());
            iface.asyncCall("setBackgroundVisible", true);
        }
        //deepinv23 quit!
        else if(dApp->m_isDDE23)
        {
            if(dApp->m_startDesktop)
            {
                //dde-dconfig --set -a org.deepin.dde.file-manager -r org.deepin.dde.file-manager.plugins -k desktop.blackList -v "[]"
                QStringList arguments;
                arguments << "--set" << "-a" << "org.deepin.dde.file-manager" << "-r" << "org.deepin.dde.file-manager.plugins" << "-k" << "desktop.blackList" << "-v" << "[]";
                QProcess process;
                process.start("dde-dconfig", arguments);
                process.waitForFinished(-1);
                qDebug()<<"dde-dconfig end";
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
                QThread * thread = QThread::create([ = ]() {
                    QProcess::execute("killall dde-desktop");
                    QString strPath = QString("dde-desktop");
                    QProcess pro;
                    pro.startDetached(strPath);
                    qDebug() << "启动失败: " ;
                });
                thread->start();
#endif
            }
        }
    }
    dApp->exit();
}



void settingWindow::on_mainWeb_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper")));
}

void settingWindow::on_history_clicked()
{
    if (!m_history) {
        m_history = new historyWidget();
        m_history->resize(800, 600);
        m_history->showNormal();
        m_history->move(QGuiApplication::primaryScreen()->geometry().center() - m_history->rect().center());
    } else {
        m_history->showNormal();
        m_history->activateWindow();
    }
}

void settingWindow::slotWallPaper(const QString &path)
{
    if (!path.isEmpty()) {
        dApp->m_currentPath = path;
        ui->pathEdit->setText(path);
        Q_EMIT dApp->setPlayPath(ui->pathEdit->text());
        if(QGuiApplication::screens().size() > 1 && !dApp->m_isPath2)
        {
            Q_EMIT dApp->setPlayPath2(ui->pathEdit->text());
        }
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->addLocalPaths(QStringList(dApp->m_currentPath));
        dApp->m_allPath = dApp->convertQStringListToSet( dApp->m_allPath).values();

        saveSettings();
        on_Slider_valueChanged(m_voiceVolume);
    }
}
void settingWindow::slotWallPaper2(const QString &path)
{
    if (!path.isEmpty()) {
        dApp->m_currentPath2 = path;
        ui->pathEdit2->setText(path);
        Q_EMIT dApp->setPlayPath2(ui->pathEdit2->text());
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail2->setPixmap(pix);
        }
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->addLocalPaths(QStringList(dApp->m_currentPath2));
        dApp->m_allPath = dApp->convertQStringListToSet( dApp->m_allPath).values();
        saveSettings();
        on_Slider_valueChanged(m_voiceVolume);

        ui->checkBox2Inde->setChecked(true);
        ui->pathBtn2->setVisible(true);
        ui->pathEdit2->setVisible(true);
        ui->pixThumbnail2->setVisible(true);
        ui->setBtn2->setVisible(true);
    }
}


void settingWindow::on_giteeWeb_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://gitee.com/liuminghang/fantascene-dynamic-wallpaper/releases")));
}

void settingWindow::on_githubWeb_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/releases")));
}

void settingWindow::on_bugBtn_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/issues/new")));
}

void settingWindow::on_videoBLCombox_activated(const QString &arg1)
{
    ui->videoBLEdit->setVisible(false);
    ui->videoBZDY->setVisible(false);
    double value = -1.0;
    if (arg1 == tr("default")) {
        value = -1.0;
    } else if (arg1.contains("4:3")) {
        value = 1.33333333;
    } else if (arg1.contains("16:9")) {
        value = 1.77777777;
    } else if (arg1.contains("16:10")) {
        value = 1.6;
    } else if (arg1.contains("1.85:1")) {
        value = 1.85;
    } else if (arg1.contains("2.35:1")) {
        value = 2.35;
    } else if (arg1 == tr("custom")) {
        ui->videoBLEdit->setVisible(true);
        ui->videoBZDY->setVisible(true);
        value = ui->videoBLEdit->text().toDouble();
    }
    ui->videoBLEdit->setText(QString::number(value));
    m_videoAspect = value;
    m_videoASpectStr = arg1;
    Q_EMIT dApp->setMpvValue("video-aspect", QString::number(value));

    saveSettings();
}

void settingWindow::on_videoBZDY_clicked()
{
    double value = ui->videoBLEdit->text().toDouble();
    m_videoAspect = value;
    Q_EMIT dApp->setMpvValue("video-aspect", QString::number(value));
    saveSettings();
}

bool settingWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() == "pathEdit") {
        if (event->type() == QEvent::DragEnter) {
            ui->pathEdit->setText("");
            return true;
        }
        event->accept();
    }
    if (event->type() == QEvent::Drop) {

    }
    if (event->type() == QEvent::DragMove) {
        event->accept();
    }
    if (event->type() == QEvent::DragEnter) {
        QList<QUrl> urls = static_cast<QDropEvent *>(event)->mimeData()->urls();
        if (urls.isEmpty()) {
            return false;
        }

        QStringList paths;
        for (QUrl url : urls) {
            const QString path = url.toLocalFile();
            paths << path;
        }

        if (!paths.isEmpty()) {
            //            ui->mainImageView->openImage(paths.at(0));
            //            ui->basicImageView->openImage(paths.at(0));
            ui->pathEdit->setText(paths.at(0));
            QPixmap pix = dApp->getThumbnail(paths.at(0));
            if (!pix.isNull()) {
                ui->pixThumbnail->setPixmap(pix);
            }
        }

        event->accept();
        return true;
    }

    if (event->type() == QEvent::Resize && this->isVisible()) {

    }

    return false;
}

void settingWindow::slotMoreSettingSave()
{
    on_checkBox_stateChanged(dApp->m_moreData.isAuto);

    dApp->setMpvValue("hwdec", dApp->m_moreData.hwdec);
    slotShowDesktopIcon(dApp->m_moreData.isShowDesktopIcon);
    on_setBtn_clicked();
    on_setBtn2_clicked();
    saveSettings();

}

void settingWindow::on_pathEdit_textChanged(const QString &arg1)
{
    QPixmap pix = dApp->getThumbnail(arg1);
    if (!pix.isNull()) {
        ui->pixThumbnail->setPixmap(pix);
    }
}

void settingWindow::on_checkBox_stateChanged(int arg1)
{

    // 如果平台不是x11,则退出
    if ( QGuiApplication::platformName() != QLatin1String("xcb")) {
        return ;
    }
#ifdef Q_OS_LINUX
    //问题很多
    if (arg1 == 0) {
        dApp->m_moreData.isAuto = 0;
        m_stopx11Thread = true;
        if (m_future.isRunning()) {
            //            m_x11thread->wait();
            //            m_x11thread->quit();
            //            m_x11thread->terminate();
        }
        if (dApp->m_isNoMpvPause) {
            dApp->setMpvPlay();
        }
    } else {
        m_stopx11Thread = false;
        dApp->m_moreData.isAuto = 1;
        if (!m_future.isRunning()) {
            m_future = QtConcurrent::run([=](){
                int screenwidth = QGuiApplication::primaryScreen()->geometry().width() - 150;
                int screenheight = QGuiApplication::primaryScreen()->geometry().height() - 150;
                while (!m_stopx11Thread) {
                    if (dApp->m_isNoMpvPause) {
                        int index = 0;

                        for (auto wid : currentWorkWindow()) {

                            QRect rect = geometry(wid);
                            Qt::WindowState state = getWindowState(wid);

                            if (state == Qt::WindowState::WindowMaximized || state == Qt::WindowState::WindowFullScreen) {


                                int wwidth = rect.width();
                                int wheight = rect.height();

                                if (wwidth > screenwidth && wheight > screenheight) {

                                    index++;
                                }

                            }

                        }
                        if (0 == index && !dApp->m_currentIsPlay &&  dApp->m_isNoMpvPause) {
                            dApp->setMpvPlay();
                        } else if (0 != index &&  dApp->m_currentIsPlay) {
                            dApp->setMpvpause();
                        }

                    }
                    QThread::msleep(2000);
                }
            });
//            m_x11thread->start();
        }
    }
    saveSettings();
#endif
}


#include "moresetting.h"
void settingWindow::on_moreSettingBtn_clicked()
{
    if (!m_moreSetting) {
        m_moreSetting = new MoreSetting();
    }
    m_moreSetting->setData(dApp->m_moreData);

    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();

    m_moreSetting->show();
    m_moreSetting->move(screenWidth/2 - m_moreSetting->width()/2,screenHeight/2 - m_moreSetting->height()/2);
}

void settingWindow::on_pluginBtn_clicked()
{
    if (!m_wallpaperEnginePlugin) {
        m_wallpaperEnginePlugin = new wallpaperEnginePlugin();
        m_wallpaperEnginePlugin->showNormal();
        m_wallpaperEnginePlugin->move(QGuiApplication::primaryScreen()->geometry().center() - m_wallpaperEnginePlugin->rect().center());
    } else {
        m_wallpaperEnginePlugin->showNormal();
        m_wallpaperEnginePlugin->activateWindow();
    }
}

void settingWindow::activeWindow()
{
    this->show();
    if (m_parentMainWindow) {
        m_parentMainWindow->resize(500, 300);
        m_parentMainWindow->show();
        m_parentMainWindow->activateWindow();
    }
}

void settingWindow::on_tansparency_slider_valueChanged(int value)
{
    Q_EMIT dApp->sigSetTransparency(value);
}

void settingWindow::slotShowDesktopIcon(bool isIcon)
{
    if (m_wallpaper) {
        m_wallpaper->setIconVisble(isIcon);
    }
}

void settingWindow::slotTimerSaveSettings()
{
    int iconSize = 0;
    int iSort= 0;

    if(IniManager::instance()->contains("WallPaper/IconSize"))
    {
        iconSize = IniManager::instance()->value("WallPaper/IconSize").toInt();
    }
    if(IniManager::instance()->contains("WallPaper/SortFilter"))
    {
        iSort = IniManager::instance()->value("WallPaper/SortFilter").toInt();
    }

    IniManager::instance()->setValue("WallPaper/ScrrenNumber", m_crrenNumber);
    IniManager::instance()->setValue("WallPaper/isAutoStart", m_isAutoStart);

    IniManager::instance()->setValue("WallPaper/CurrentPath", dApp->m_currentPath);
    IniManager::instance()->setValue("WallPaper/IsPath2", dApp->m_isPath2);
    IniManager::instance()->setValue("WallPaper/CurrentPath2", dApp->m_currentPath2);

    IniManager::instance()->setValue("WallPaper/Mode", ui->comboBox->currentText());
    IniManager::instance()->setValue("WallPaper/widthPY", dApp->m_manual.x());
    IniManager::instance()->setValue("WallPaper/heightPY", dApp->m_manual.y());
    IniManager::instance()->setValue("WallPaper/width", dApp->m_manual.width());
    IniManager::instance()->setValue("WallPaper/height", dApp->m_manual.height());
    IniManager::instance()->setValue("WallPaper/voiceVolume", m_voiceVolume);
    IniManager::instance()->setValue("WallPaper/videoAspect", m_videoAspect);
    IniManager::instance()->setValue("WallPaper/videoAspectStr", m_videoASpectStr);
    IniManager::instance()->setValue("WallPaper/videoAutoMode", dApp->m_moreData.isAuto);
    IniManager::instance()->setValue("WallPaper/fps", dApp->m_moreData.fps);
    IniManager::instance()->setValue("WallPaper/hwdec", dApp->m_moreData.hwdec);
    IniManager::instance()->setValue("WallPaper/vo", dApp->m_moreData.vo);
    IniManager::instance()->setValue("WallPaper/desktopShow", dApp->m_moreData.isShowDesktopIcon);
    IniManager::instance()->setValue("WallPaper/topDefault", dApp->m_moreData.isTop);
    IniManager::instance()->setValue("WallPaper/wallpaperEnginePath", dApp->m_wallpaperEnginePath);
    IniManager::instance()->setValue("WallPaper/isPlayList", dApp->m_isPlayList);
    IniManager::instance()->setValue("WallPaper/playlistTimer", dApp->m_PlaylistTimer);

    if(iconSize>0)
    {
        IniManager::instance()->setValue("WallPaper/IconSize", iconSize);
    }
    if(iSort>=0)
    {
        IniManager::instance()->setValue("WallPaper/SortFilter", iSort);
    }

    IniManager::instance()->setValue("Wallpaper/EventPenetration",dApp->m_moreData.isEventPenetration);

    IniManager::instance()->setValue("Wallpaper/DesktopFontColor",dApp->m_moreData.fontColor);
    dApp->setAppColor(dApp->m_moreData.fontColor);

    int indexLocal = 1;
//    //去重
//    dApp->m_allPath = dApp->convertQStringListToSet( dApp->m_allPath).values();
//    for (QString str : dApp->m_allPath) {
//        IniManager::instance()->setValue("Movie/localPath" + QString::number(indexLocal++), str);
//    }

//    //去重
//    int playlistIndex = 1;
//    dApp->m_playlistPath = dApp->m_playlistPath.toSet().toList();
//    for (QString str : dApp->m_playlistPath) {
//        IniManager::instance()->setValue("Movie/playlistPath" + QString::number(playlistIndex++), str);
//    }
}

#ifdef Q_OS_LINUX
xcb_atom_t settingWindow::internAtom(xcb_connection_t *connection, const char *name, bool only_if_exists)
{
    if (!name || *name == 0)
        return XCB_NONE;

    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, only_if_exists, strlen(name), name);
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection, cookie, 0);

    if (!reply)
        return XCB_NONE;

    xcb_atom_t atom = reply->atom;
    free(reply);

    return atom;
}

QVector<xcb_window_t> settingWindow::getWindows() const
{
    QVector<xcb_window_t> window_list_stacking;
    XDefaultRootWindow(static_cast<Display *>(dApp->getDisplay()));

    xcb_window_t root /*= XDefaultRootWindow(display)->primaryScreen()->root()*/;
    int offset = 0;
    int remaining = 0;

    do {
        xcb_atom_t atomtt = internAtom(dApp->getXcb_connection_t(), "_NET_CLIENT_LIST_STACKING");
        xcb_get_property_cookie_t cookie = xcb_get_property(dApp->getXcb_connection_t(), false,  XDefaultRootWindow(static_cast<Display *>(dApp->getDisplay())),
                                                            atomtt,
                                                            XCB_ATOM_WINDOW, offset, 1024);
        xcb_get_property_reply_t *reply = xcb_get_property_reply(dApp->getXcb_connection_t(), cookie, NULL);
        if (!reply)
            break;

        remaining = 0;

        if (reply->type == XCB_ATOM_WINDOW && reply->format == 32) {
            int len = xcb_get_property_value_length(reply) / sizeof(xcb_window_t);
            xcb_window_t *windows = (xcb_window_t *)xcb_get_property_value(reply);
            int s = window_list_stacking.size();
            window_list_stacking.resize(s + len);
            memcpy(window_list_stacking.data() + s, windows, len * sizeof(xcb_window_t));

            remaining = reply->bytes_after;
            offset += len;
        }

        free(reply);
    } while (remaining > 0);

    return window_list_stacking;
}
qint32 settingWindow::getWorkspaceForWindow(quint32 WId, xcb_connection_t *connection)
{
    xcb_get_property_cookie_t cookie = xcb_get_property(connection, false, WId,
                                                        internAtom(connection, "_NET_WM_DESKTOP"), XCB_ATOM_CARDINAL, 0, 1);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
                xcb_get_property_reply(connection, cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 1) {
        return *(qint32 *)xcb_get_property_value(reply.data());
    }

    return 0;
}

QVector<uint> settingWindow::getCurrentWorkspaceWindows()
{

    qint32 current_workspace = 0;

    xcb_get_property_cookie_t cookie = xcb_get_property(dApp->getXcb_connection_t(), false,
                                                        winId(),
                                                        internAtom(dApp->getXcb_connection_t(), "_NET_CURRENT_DESKTOP"), XCB_ATOM_CARDINAL, 0, 1);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
                xcb_get_property_reply(dApp->getXcb_connection_t(), cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 1) {
        current_workspace = *(qint32 *)xcb_get_property_value(reply.data());
    }

    QVector<uint> windows;

    for (uint32_t WId : getWindows()) {
        qint32 ws = getWorkspaceForWindow(WId,dApp->getXcb_connection_t());

        if (ws < 0 || ws == current_workspace) {
            windows << WId;
        }
    }

    return windows;
}

QRect settingWindow::geometry(WId id) const
{
    XWindowAttributes bute;
    XGetWindowAttributes(static_cast<Display *>(dApp->getDisplay()), id, &bute);
    QRect rect;
    if (&bute) {
        rect.setX(bute.x);
        rect.setY(bute.y);
        rect.setWidth(bute.width);
        rect.setHeight(bute.height);
    }
    return rect;
}

Qt::WindowState settingWindow::getWindowState(WId id)
{
    xcb_window_t window = id;
    Qt::WindowState newState = Qt::WindowNoState;
    const xcb_get_property_cookie_t get_cookie =
            xcb_get_property(dApp->getXcb_connection_t(), 0, window, m_ewmh_connection._NET_WM_STATE,
                             XCB_ATOM_ANY, 0, 1024);

    xcb_get_property_reply_t *reply =
            xcb_get_property_reply(dApp->getXcb_connection_t(), get_cookie, NULL);

    if (reply) {
        const quint32 *data = (const quint32 *)xcb_get_property_value(reply);
        if (data[0] == m_ewmh_connection._NET_WM_STATE_FULLSCREEN) {
            newState = Qt::WindowFullScreen;
        } else if (data[0] == m_ewmh_connection._NET_WM_STATE_MAXIMIZED_VERT || data[0] == m_ewmh_connection._NET_WM_STATE_MAXIMIZED_HORZ) {
            newState = Qt::WindowMaximized;
            if (data[2] == m_ewmh_connection._NET_WM_STATE_HIDDEN) {
                newState = Qt::WindowMinimized;
            }
        } else if (data[0] == m_ewmh_connection._NET_WM_STATE_HIDDEN) {
            newState = Qt::WindowMinimized;
        }
    }
    free(reply);

    return newState;
}

void settingWindow::initAtom()
{
    if(QGuiApplication::platformName() == "xcb") {
        xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(dApp->getXcb_connection_t(), &m_ewmh_connection);
        xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);
    }
    else
    {

    }
}


uint32_t settingWindow::searchWindowType(int wid)
{
    uint32_t reId = 0;
    QMutexLocker locker(&m_mutex);
    //    if (m_cookie) {

    xcb_get_property_cookie_t cooke = xcb_ewmh_get_wm_window_type(&m_ewmh_connection, wid);

    xcb_ewmh_get_atoms_reply_t name;
    xcb_generic_error_t *error_t = new xcb_generic_error_t;

    xcb_ewmh_get_wm_window_type_reply(&m_ewmh_connection, cooke, &name, &error_t);

    if (error_t) {
        delete error_t;
        error_t = NULL;
        return 381;
    } else {

    }
    if (name.atoms && name.atoms_len <= 10) {
        reId = name.atoms[0];
    }

    return reId;
}
#endif
void settingWindow::initWallpaperWidget()
{
    m_wallpaper = new Wallpaper(this->getCurrentPath(), this->getCurrentNumber());
    dApp->setDesktopTransparent();

    DBusWallpaperService *dbusInter = new DBusWallpaperService(m_wallpaper);
    Q_UNUSED(dbusInter);

    QDBusConnection::sessionBus().registerService("com.deepin.dde.fantascene");
    QDBusConnection::sessionBus().registerObject("/com/deepin/dde/fantascene", "com.deepin.dde.fantascene", m_wallpaper);
}



void settingWindow::on_localBtn_clicked()
{
    if (!m_local) {
        m_local = new LocalWidget();
        m_local->resize(800, 600);
        m_local->showNormal();
        m_local->move(QGuiApplication::primaryScreen()->geometry().center() - m_local->rect().center());
    } else {
        m_local->showNormal();
        m_local->activateWindow();
    }
}
#include "listview/onlineclient.h"
void settingWindow::on_liveBtn_clicked()
{
    if(!m_downloadWidget)
    {
        m_downloadWidget =new DownloadWidget();
    }
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();

    m_downloadWidget->show();
    m_downloadWidget->move(screenWidth/2 - m_downloadWidget->width()/2,screenHeight/2 - m_downloadWidget->height()/2);
    m_downloadWidget->activateWindow();
}

void settingWindow::on_checkBox2Inde_stateChanged(int arg1)
{
    ui->pathBtn2->setVisible(arg1);
    ui->pathEdit2->setVisible(arg1);
    ui->pixThumbnail2->setVisible(arg1);
    ui->setBtn2->setVisible(arg1);

    dApp->m_isPath2 = arg1 ;
    IniManager::instance()->setValue("WallPaper/IsPath2", dApp->m_isPath2);
    if(dApp->m_isPath2 && !dApp->m_currentPath2.isEmpty())
    {
        Q_EMIT dApp->setPlayPath2(ui->pathEdit2->text());
    }
    else
    {
        Q_EMIT dApp->setPlayPath2(ui->pathEdit->text());
    }
}

void settingWindow::on_pathBtn2_clicked()
{
    QString path ;
    if(dApp->m_isNoMpvPause)
    {
        Q_EMIT dApp->setMpvpause();
        dApp->m_isNoMpvPause = false;
        path = QFileDialog::getOpenFileUrl().toLocalFile();

        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
    }
    if (!path.isEmpty()) {
        ui->pathEdit2->setText(path);
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail2->setPixmap(pix);
        }
    }
}

void settingWindow::on_pathEdit2_textChanged(const QString &arg1)
{
    QPixmap pix = dApp->getThumbnail(arg1);
    if (!pix.isNull()) {
        ui->pixThumbnail2->setPixmap(pix);
    }
}

void settingWindow::on_setBtn2_clicked()
{
    if (ui->pathEdit2->text() != nullptr) {
        dApp->m_currentPath2 = ui->pathEdit2->text();
        dApp->m_currentPath2 = dApp->m_currentPath2.replace("file://", "");
        Q_EMIT dApp->setPlayPath2(ui->pathEdit2->text());
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        saveSettings();
        Q_EMIT dApp->addPaperView(dApp->m_currentPath2);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath2);
        if (!pix.isNull()) {
            ui->pixThumbnail2->setPixmap(pix);
        }
        dApp->addLocalPaths(QStringList(dApp->m_currentPath2));
    }
}

void settingWindow::on_downBtn_clicked()
{
    if(!m_onlineWidget)
    {
        m_onlineWidget = new OnlineClient();
    }
    int screenWidth = QGuiApplication::primaryScreen()->geometry().width();
    int screenHeight = QGuiApplication::primaryScreen()->geometry().height();

    m_onlineWidget->show();
    m_onlineWidget->move(screenWidth/2 - m_onlineWidget->width()/2,screenHeight/2 - m_onlineWidget->height()/2);
    m_onlineWidget->activateWindow();
}

