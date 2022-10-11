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
#include <QDesktopWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>


#include <QMutexLocker>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <xcb/shape.h>


#include "listview/historywidget.h"
#include "listview/wallpaperengineplugin.h"


#define SETTINGPATH "config.ini"
const QString CONFIG_PATH =   QDir::homePath() +
        "/.config/fantascene-dynamic-wallpaper/config.ini";


settingWindow::settingWindow(QWidget *parent, QMainWindow *mainWindow) :
    QWidget(parent),
    m_parentMainWindow(mainWindow),
    ui(new Ui::settingWindow)
{
    ui->setupUi(this);
    readSettings();


    ui->tansparency_slider->hide();
    ui->label_8->hide();
    /*qApp->*/installEventFilter(this);
    ui->pathEdit->installEventFilter(this);
    ui->pathEdit->setAcceptDrops(true);
    setAcceptDrops(true);

    m_traymenu = new QMenu();
    QAction *exitAction = new QAction(m_traymenu);
    exitAction->setText(tr("Exit"));
    connect(exitAction, &QAction::triggered, this, &settingWindow::quitApp);
    connect(dApp, &Application::quitApp, this, &settingWindow::quitApp, Qt::DirectConnection);

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

    QAction *maingithub = new QAction(m_traymenu);
    maingithub->setText(tr("github"));
    connect(maingithub, &QAction::triggered, this, [ = ] {
        QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/")));
    });

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
            if (m_parentMainWindow) {
                m_parentMainWindow->resize(500, 300);
                m_parentMainWindow->show();
                m_parentMainWindow->activateWindow();
            }
        }
    });

    connect(dApp, &Application::pathChanged, this, &settingWindow::pathChanged);
    connect(dApp, &Application::setWallPaper, this, &settingWindow::slotWallPaper);

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

    initAtom();

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

    QSettings settings(CONFIG_PATH, QSettings::IniFormat);

    dApp->m_currentPath = settings.value("WallPaper/CurrentPath").toString();
    m_crrenNumber = settings.value("WallPaper/ScrrenNumber").toInt(); //1-2
    m_isAutoStart = settings.value("WallPaper/isAutoStart").toInt();
    int widthPY = settings.value("WallPaper/widthPY").toInt();
    int heightPY = settings.value("WallPaper/heightPY").toInt();
    int width = settings.value("WallPaper/width").toInt();
    int height = settings.value("WallPaper/height").toInt();
    m_currentMode = settings.value("WallPaper/Mode").toString();
    m_voiceVolume = settings.value("WallPaper/voiceVolume").toInt();
    m_videoAspect = settings.value("WallPaper/videoAspect").toDouble();
    m_videoASpectStr = settings.value("WallPaper/videoAspectStr").toString();

    dApp->m_moreData.isAuto = settings.value("WallPaper/videoAutoMode").toInt();
    dApp->m_moreData.fps = settings.value("WallPaper/fps").toInt();
    dApp->m_moreData.hwdec = settings.value("WallPaper/hwdec").toString();
    dApp->m_wallpaperEnginePath = settings.value("WallPaper/wallpaperEnginePath").toString();
    if(settings.contains("WallPaper/isPlayList")){
        dApp->m_isPlayList = settings.value("WallPaper/isPlayList").toBool();
    }
    dApp->m_PlaylistTimer = settings.value("WallPaper/playlistTimer").toInt();
    if(settings.contains("WallPaper/desktopShow")){
        dApp->m_moreData.isShowDesktopIcon = settings.value("WallPaper/desktopShow").toBool();
    }

    dApp->setisPlayList(dApp->m_isPlayList);
    dApp->setPlayListTimer(dApp->m_PlaylistTimer);

    if (dApp->m_moreData.hwdec.isEmpty()) {
        dApp->m_moreData.hwdec = "gpu";
    }
    ui->videoBLEdit->setText(QString::number(m_videoAspect));
    ui->videoBLCombox->setCurrentText(m_videoASpectStr);

    //取值本地地址
    QString strLocalPath;
    int localIndex = 1;
    do {
        strLocalPath = settings.value("Movie/localPath" + QString::number(localIndex++)).toString();
        if (nullptr != strLocalPath) {
            if (!dApp->m_allPath.contains(strLocalPath)) {
                dApp->m_allPath.push_back(strLocalPath);
                Q_EMIT dApp->addPaperView(strLocalPath);
            }
        }
    } while (nullptr != strLocalPath);

    //取值本地地址
    QString strPlaylistPath;
    int playlistIndex = 1;
    do {
        strPlaylistPath = settings.value("Movie/playlistPath" + QString::number(playlistIndex++)).toString();
        if (nullptr != strPlaylistPath) {
            if (!dApp->m_playlistPath.contains(strPlaylistPath)) {
                dApp->m_playlistPath.push_back(strPlaylistPath);
                Q_EMIT dApp->addplaylist(strPlaylistPath);
            }
        }
    } while (nullptr != strPlaylistPath);


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

QVector <WId> settingWindow::currentWorkWindow()
{
    //    QWindowList m_windowList;
    //    for (QWindow *w : m_windowList) {
    //        w->deleteLater();
    //    }
    //    m_windowList.clear();
    QList<WId> currentApplicationList;
    const QWindowList &list = qApp->allWindows();
    currentApplicationList.reserve(list.size());
    for (auto window : list) {
        if (window->property("_q_foreignWinId").isValid()) continue;

        currentApplicationList.append(window->winId());
    }
    QVector<quint32> winList;
    //    QFunctionPointer wmClientList = Q_NULLPTR;
    //#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    //    QByteArray function = "_d_getCurrentWorkspaceWindows";
    //    wmClientList = qApp->platformFunction(function);
    //#endif
    //    if (!wmClientList) {
    //        winList = QVector<quint32>();
    //    } else {
    //        winList = reinterpret_cast<QVector<quint32>(*)()>(wmClientList)();
    //    }
    winList = getCurrentWorkspaceWindows();



    for (WId wid : winList) {

        if (currentApplicationList.contains(wid)) {
            continue;
        }
        if (m_windowList.contains(wid)) {
            continue;
        }
        //        QWindow *w = new QWindow();
        //        w->setFlags(Qt::ForeignWindow);
        //        w->setProperty("_q_foreignWinId", QVariant::fromValue(wid));
        //        w->create();
        m_windowList.push_back(wid);
    }
    return m_windowList;
}

void settingWindow::setScreenMode(const QString &arg)
{
    Q_EMIT dApp->setScreenMode(arg);
}

void settingWindow::on_pathBtn_clicked()
{
    QString path = QFileDialog::getOpenFileName();
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
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->m_allPath.push_back(dApp->m_currentPath);
        saveSettings();
        Q_EMIT dApp->addPaperView(dApp->m_currentPath);
        QPixmap pix = dApp->getThumbnail(dApp->m_currentPath);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
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
    QSettings settings(CONFIG_PATH, QSettings::IniFormat);
    settings.setValue("WallPaper/voiceVolume", m_voiceVolume);
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
    on_startBtn_clicked();

}

void settingWindow::on_autoStart_clicked()
{

    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/";
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
    saveSettings();
    //#endif
    //dbus关闭壁纸透明

    //    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground true");
    m_stopx11Thread = true;
    if (m_x11thread) {
        m_x11thread->wait();
        m_x11thread->quit();
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
        m_history->move(qApp->desktop()->screen()->rect().center() - m_history->rect().center());
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
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
        Q_EMIT dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->m_allPath.push_back(dApp->m_currentPath);
        dApp->m_allPath = dApp->m_allPath.toSet().toList();
        saveSettings();
        on_Slider_valueChanged(m_voiceVolume);
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
    //问题很多
    if (arg1 == 0) {
        dApp->m_moreData.isAuto = 0;
        m_stopx11Thread = true;
        if (m_x11thread) {
            //            m_x11thread->wait();
            //            m_x11thread->quit();
            //            m_x11thread->terminate();
            m_x11thread = nullptr;
        }
        if (dApp->m_isNoMpvPause) {
            dApp->setMpvPlay();
        }
    } else {
        m_stopx11Thread = false;
        dApp->m_moreData.isAuto = 1;
        if (!m_x11thread) {
            m_x11thread = QThread::create([ = ]() {
                int screenwidth = qApp->desktop()->screenGeometry().width() - 10;
                int screenheight = qApp->desktop()->screenGeometry().height() - 150;
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
            m_x11thread->start();
        }
    }
    saveSettings();
}


#include "moresetting.h"
void settingWindow::on_moreSettingBtn_clicked()
{
    if (!m_moreSetting) {
        m_moreSetting = new MoreSetting();
    }
    m_moreSetting->setData(dApp->m_moreData);
    m_moreSetting->show();
}

void settingWindow::on_pluginBtn_clicked()
{
    if (!m_wallpaperEnginePlugin) {
        m_wallpaperEnginePlugin = new wallpaperEnginePlugin();
        m_wallpaperEnginePlugin->showNormal();
        m_wallpaperEnginePlugin->move(qApp->desktop()->screen()->rect().center() - m_wallpaperEnginePlugin->rect().center());
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
    QSettings settings(CONFIG_PATH, QSettings::IniFormat);
    settings.clear();
    settings.setValue("WallPaper/ScrrenNumber", m_crrenNumber);
    settings.setValue("WallPaper/isAutoStart", m_isAutoStart);
    settings.setValue("WallPaper/CurrentPath", dApp->m_currentPath);
    settings.setValue("WallPaper/Mode", ui->comboBox->currentText());
    settings.setValue("WallPaper/widthPY", dApp->m_manual.x());
    settings.setValue("WallPaper/heightPY", dApp->m_manual.y());
    settings.setValue("WallPaper/width", dApp->m_manual.width());
    settings.setValue("WallPaper/height", dApp->m_manual.height());
    settings.setValue("WallPaper/voiceVolume", m_voiceVolume);
    settings.setValue("WallPaper/videoAspect", m_videoAspect);
    settings.setValue("WallPaper/videoAspectStr", m_videoASpectStr);
    settings.setValue("WallPaper/videoAutoMode", dApp->m_moreData.isAuto);
    settings.setValue("WallPaper/fps", dApp->m_moreData.fps);
    settings.setValue("WallPaper/hwdec", dApp->m_moreData.hwdec);
    settings.setValue("WallPaper/desktopShow", dApp->m_moreData.isShowDesktopIcon);
    settings.setValue("WallPaper/wallpaperEnginePath", dApp->m_wallpaperEnginePath);
    settings.setValue("WallPaper/isPlayList", dApp->m_isPlayList);
    settings.setValue("WallPaper/playlistTimer", dApp->m_PlaylistTimer);

    int indexLocal = 1;
    //去重
    dApp->m_allPath = dApp->m_allPath.toSet().toList();
    for (QString str : dApp->m_allPath) {
        settings.setValue("Movie/localPath" + QString::number(indexLocal++), str);
    }

    //去重
    int playlistIndex = 1;
    dApp->m_playlistPath = dApp->m_playlistPath.toSet().toList();
    for (QString str : dApp->m_playlistPath) {
        settings.setValue("Movie/playlistPath" + QString::number(playlistIndex++), str);
    }
}


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
    Display *display = QX11Info::display();
    XDefaultRootWindow(display);
    xcb_window_t root /*= XDefaultRootWindow(display)->primaryScreen()->root()*/;
    int offset = 0;
    int remaining = 0;
    xcb_connection_t *xcb_connection = QX11Info::connection();

    do {
        xcb_atom_t atomtt = internAtom(xcb_connection, "_NET_CLIENT_LIST_STACKING");
        xcb_get_property_cookie_t cookie = xcb_get_property(xcb_connection, false,  XDefaultRootWindow(display),
                                                            atomtt,
                                                            XCB_ATOM_WINDOW, offset, 1024);
        xcb_get_property_reply_t *reply = xcb_get_property_reply(xcb_connection, cookie, NULL);
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
qint32 settingWindow::getWorkspaceForWindow(quint32 WId)
{
    xcb_get_property_cookie_t cookie = xcb_get_property(QX11Info::connection(), false, WId,
                                                        internAtom(QX11Info::connection(), "_NET_WM_DESKTOP"), XCB_ATOM_CARDINAL, 0, 1);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
                xcb_get_property_reply(QX11Info::connection(), cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 1) {
        return *(qint32 *)xcb_get_property_value(reply.data());
    }

    return 0;
}

QVector<uint> settingWindow::getCurrentWorkspaceWindows()
{
    qint32 current_workspace = 0;

    xcb_get_property_cookie_t cookie = xcb_get_property(QX11Info::connection(), false,
                                                        winId(),
                                                        internAtom(QX11Info::connection(), "_NET_CURRENT_DESKTOP"), XCB_ATOM_CARDINAL, 0, 1);
    QScopedPointer<xcb_get_property_reply_t, QScopedPointerPodDeleter> reply(
                xcb_get_property_reply(QX11Info::connection(), cookie, NULL));
    if (reply && reply->type == XCB_ATOM_CARDINAL && reply->format == 32 && reply->value_len == 1) {
        current_workspace = *(qint32 *)xcb_get_property_value(reply.data());
    }

    QVector<uint> windows;

    for (uint32_t WId : getWindows()) {
        qint32 ws = getWorkspaceForWindow(WId);

        if (ws < 0 || ws == current_workspace) {
            windows << WId;
        }
    }

    return windows;
}

QRect settingWindow::geometry(WId id) const
{

    XWindowAttributes bute;
    XGetWindowAttributes(QX11Info::display(), id, &bute);
    QRect rect;
    if (&bute) {
        rect.setX(bute.x);
        rect.setY(bute.y);
        rect.setWidth(bute.width);
        rect.setHeight(bute.height);
    }
    return rect;
}
#include <X11/Xatom.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
Qt::WindowState settingWindow::getWindowState(WId id)
{

    xcb_window_t window = id;
    Qt::WindowState newState = Qt::WindowNoState;
    const xcb_get_property_cookie_t get_cookie =
            xcb_get_property(QX11Info::connection(), 0, window, m_ewmh_connection._NET_WM_STATE,
                             XCB_ATOM_ANY, 0, 1024);

    xcb_get_property_reply_t *reply =
            xcb_get_property_reply(QX11Info::connection(), get_cookie, NULL);

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
    xcb_intern_atom_cookie_t *cookie = xcb_ewmh_init_atoms(QX11Info::connection(), &m_ewmh_connection);
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);
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

void settingWindow::initWallpaperWidget()
{
    m_wallpaper = new Wallpaper(this->getCurrentPath(), this->getCurrentNumber());
    dApp->setDesktopTransparent();

    DBusWallpaperService *dbusInter = new DBusWallpaperService(m_wallpaper);
    Q_UNUSED(dbusInter);

    QDBusConnection::sessionBus().registerService("com.deepin.dde.fantascene");
    QDBusConnection::sessionBus().registerObject("/com/deepin/dde/fantascene", "com.deepin.dde.fantascene", m_wallpaper);
}


