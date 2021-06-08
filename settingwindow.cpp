#include "settingwindow.h"
#include "ui_settingwindow.h"
#include <QSystemTrayIcon>
#include <QFileDialog>
#include <QMenu>
#include <DMainWindow>
#include <QProcess>
#include <QThread>
#include <QSettings>
#include <DDesktopEntry>
#include <QCoreApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDropEvent>
#include <QMimeData>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "listview/historywidget.h"
#include "listview/wallpaperengineplugin.h"

#include <DTitlebar>

DCORE_USE_NAMESPACE

#define SETTINGPATH "config.ini"
const QString CONFIG_PATH =   QDir::homePath() +
                              "/.config/fantascene-dynamic-wallpaper/config.ini";


settingWindow::settingWindow(QWidget *parent, DMainWindow *mainWindow) :
    QWidget(parent),
    m_parentMainWindow(mainWindow),
    ui(new Ui::settingWindow)
{
    //    saveSettings();

    ui->setupUi(this);
    /*qApp->*/installEventFilter(this);
    ui->pathEdit->installEventFilter(this);
    ui->pathEdit->setAcceptDrops(true);
    setAcceptDrops(true);
    readSettings();
    m_traymenu = new QMenu();
    QAction *exitAction = new QAction(m_traymenu);
    exitAction->setText(tr("Exit"));
    connect(exitAction, &QAction::triggered, this, &settingWindow::quitApp);
    connect(dApp, &Application::quitApp, this, &settingWindow::quitApp, Qt::DirectConnection);

    QAction *setMpvPlayAction = new QAction(m_traymenu);
    setMpvPlayAction->setText(tr("Play"));
    connect(setMpvPlayAction, &QAction::triggered, this, [ = ] {
        emit dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->m_x11WindowFuscreen.clear();
    });

    QAction *setScreenshotAction = new QAction(m_traymenu);
    setScreenshotAction->setText(tr("Screenshot"));
    connect(setScreenshotAction, &QAction::triggered, this, [ = ] {
        emit dApp->sigscreenshot();
    });
    QAction *setMpvpauseAction = new QAction(m_traymenu);
    setMpvpauseAction->setText(tr("Pause"));
    connect(setMpvpauseAction, &QAction::triggered, this, [ = ] {
        emit dApp->setMpvpause();
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

//    QAction *maingithub = new QAction(m_traymenu);
//    maingithub->setText("github");
//    connect(maingithub, &QAction::triggered, this, [ = ] {
//        QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/")));
//    });

    m_traymenu->addAction(setMainViewAction);
    m_traymenu->addAction(setHistoryAction);
    m_traymenu->addAction(setMpvPlayAction);
    m_traymenu->addAction(setMpvpauseAction);
    m_traymenu->addAction(setScreenshotAction);
//    m_traymenu->addAction(maingithub);
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
        if (m_parentMainWindow)
        {
            m_parentMainWindow->resize(500, 300);
            m_parentMainWindow->show();
            m_parentMainWindow->activateWindow();
        }
    });

    ui->bugBtn->hide();
    ui->mainWeb->hide();
    ui->githubWeb->hide();
    ui->giteeWeb->hide();
//    ui->checkBox->hide();

    if (m_parentMainWindow) {
        m_aboutMenu = new QMenu();
//        QAction *aboutgit = new QAction(m_aboutMenu);
//        aboutgit->setText(tr("github"));
//        connect(aboutgit, &QAction::triggered, this, [ = ] {
//            QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/")));
//        });

        QAction *aboutMe = new QAction(m_aboutMenu);
        aboutMe->setText(tr("Latest version"));
        connect(aboutMe, &QAction::triggered, this, [ = ] {
            QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/fantascene-dynamic-wallpaper/releases")));
        });
//        m_aboutMenu->addAction(aboutgit);
        m_aboutMenu->addAction(aboutMe);

        m_parentMainWindow->titlebar()->setMenu(m_aboutMenu);
    }



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
                emit dApp->addPaperView(strLocalPath);
            }
        }
    } while (nullptr != strLocalPath);

    dApp->m_manual.setRect(widthPY, heightPY, width, height);
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

    QTimer::singleShot(300, [ = ] {
        ui->widthPY->setText(QString::number(dApp->m_manual.x()));
        ui->heightPY->setText(QString::number(dApp->m_manual.y()));
        ui->width->setText(QString::number(dApp->m_manual.width()));
        ui->height->setText(QString::number(dApp->m_manual.height()));

        if (m_voiceVolume >= 0 && m_voiceVolume < 100)
        {
            ui->Slider->setValue(m_voiceVolume);
        }
        if (!m_currentMode.isEmpty())
        {
            ui->comboBox->setCurrentText(m_currentMode);
            setScreenMode(m_currentMode);
        }
        if (m_crrenNumber > 1)
        {
            //        ui->autoisMScreen->setCheckState(Qt::Checked);
        }
        if (m_isAutoStart > 0)
        {
            ui->autoStartBox->setCheckState(Qt::Checked);
        }

        on_videoBLCombox_activated(m_videoASpectStr);


    });
    QTimer::singleShot(2000, [ = ] {
        if (dApp->m_moreData.isAuto == 1)
        {
//            ui->checkBox->setCheckState(Qt::Checked);
        }
        if (!dApp->m_moreData.hwdec.isEmpty())
        {
            dApp->setMpvValue("hwdec", dApp->m_moreData.hwdec);
        }
        on_checkBox_stateChanged(dApp->m_moreData.isAuto);
    });

    qDebug() << "x";
}

void settingWindow::saveSettings()
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
    settings.setValue("WallPaper/wallpaperEnginePath", dApp->m_wallpaperEnginePath);

    int indexLocal = 1;
    //去重
    dApp->m_allPath = dApp->m_allPath.toSet().toList();
    for (QString str : dApp->m_allPath) {
        settings.setValue("Movie/localPath" + QString::number(indexLocal++), str);
    }
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

void settingWindow::setScreenMode(const QString &arg)
{
    emit dApp->setScreenMode(arg);
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
        emit dApp->setPlayPath(ui->pathEdit->text());
        emit dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->m_allPath.push_back(dApp->m_currentPath);
        saveSettings();
        emit dApp->addPaperView(dApp->m_currentPath);
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
    emit dApp->setMpvpause();
    dApp->m_isNoMpvPause = false;
}

void settingWindow::on_stopBtn_clicked()
{
    emit dApp->setMpvstop();
    dApp->m_isNoMpvPause = false;
}

void settingWindow::on_Slider_valueChanged(int value)
{
    emit dApp->setMpvVolume(value);
    m_voiceVolume = value;

//    saveSettings();
}

void settingWindow::on_startBtn_clicked()
{
    emit dApp->setMpvPlay();
    dApp->m_isNoMpvPause = true;
    dApp->m_x11WindowFuscreen.clear();
}

void settingWindow::on_startScreen_clicked()
{
    if (index == 1) {
        index = 2;
    } else {
        index = 1;
    }
    emit dApp->setScreen(index);
    on_setBtn_clicked();
    on_startBtn_clicked();

}

void settingWindow::on_autoStart_clicked()
{

    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/deepin-dreamscene/";
    if (!QFileInfo(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/fantascene-dynamic-wallpaper.desktop").isFile())
        QProcess::execute("cp /opt/durapps/fantascene-dynamic-wallpaper/fantascene-dynamic-wallpaper.desktop " + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/");
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
        emit dApp->sigupdateGeometry();
    }
    saveSettings();
}

void settingWindow::quitApp()
{


#ifdef QT_NO_DEBUG
    QProcess::execute("killall dde-desktop");
    if (0 != dApp->m_processId) {
        QProcess::execute("kill " + QString::number(dApp->m_processId));
    }
    QThread *th = QThread::create([ = ]() {
        QProcess::execute("dde-desktop");
    });
    th->start();
    saveSettings();
#else
    saveSettings();
#endif
    //dbus关闭壁纸透明

    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground true");
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
        ui->pathEdit->setText(path);
        dApp->m_currentPath = path;
        emit dApp->setPlayPath(ui->pathEdit->text());
        QPixmap pix = dApp->getThumbnail(path);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
        emit dApp->setMpvPlay();
        dApp->m_isNoMpvPause = true;
        dApp->m_allPath.push_back(dApp->m_currentPath);
        dApp->m_allPath = dApp->m_allPath.toSet().toList();
        saveSettings();
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
    emit dApp->setMpvValue("video-aspect", QString::number(value));

    saveSettings();
}

void settingWindow::on_videoBZDY_clicked()
{
    double value = ui->videoBLEdit->text().toDouble();
    m_videoAspect = value;
    emit dApp->setMpvValue("video-aspect", QString::number(value));
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
#include "setdesktop.h"
#include <DWindowManagerHelper>
#include <DForeignWindow>
#include <QMutexLocker>
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
        dApp->m_x11WindowFuscreen.clear();
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
//                        DWindowManagerHelper::instance()->currentWorkspaceWindowIdList();
                        for (auto window : DWindowManagerHelper::instance()->currentWorkspaceWindows()) {
//                            if (wid == winId()) {
//                                continue;
//                                qDebug() << "this";
//                            }

//                            DForeignWindow *window = DForeignWindow::fromWinId(wid);
//                            //判断窗口是否有最大窗口

                            if (window->windowState() == Qt::WindowState::WindowMaximized || window->windowState() == Qt::WindowState::WindowFullScreen) {
                                //            continue;

                                int wwidth = window->frameGeometry().width();
                                int wheight = window->frameGeometry().height();

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
    if (m_parentMainWindow) {
        m_parentMainWindow->resize(500, 300);
        m_parentMainWindow->show();
        m_parentMainWindow->activateWindow();
    }
}
