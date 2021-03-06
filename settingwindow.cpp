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

DCORE_USE_NAMESPACE

#define SETTINGPATH "config.ini"
settingWindow::settingWindow(QWidget *parent, DMainWindow *mainWindow) :
    QWidget(parent),
    m_parentMainWindow(mainWindow),
    ui(new Ui::settingWindow)
{
//    saveSettings();

    ui->setupUi(this);
    readSettings();
    m_traymenu = new QMenu();
    QAction *exitAction = new QAction(m_traymenu);
    exitAction->setText("退出");
    connect(exitAction, &QAction::triggered, this, [ = ] {
        QProcess::execute("killall dde-desktop");
        QThread *th = QThread::create([ = ]()
        {
            QProcess::execute("dde-desktop");
        });
        th->start();
        saveSettings();
        dApp->exit();
    });

    QAction *setMpvPlayAction = new QAction(m_traymenu);
    setMpvPlayAction->setText("播放");
    connect(setMpvPlayAction, &QAction::triggered, dApp, &Application::setMpvPlay);

    QAction *setMpvpauseAction = new QAction(m_traymenu);
    setMpvpauseAction->setText("暂停");
    connect(setMpvpauseAction, &QAction::triggered, dApp, &Application::setMpvpause);

    //    QAction *setMpvstopAction=new QAction(m_traymenu);
    //    setMpvstopAction->setText("停止");
    //    connect(setMpvstopAction,&QAction::triggered,dApp,&Application::setMpvstop);

    m_traymenu->addAction(setMpvPlayAction);
    m_traymenu->addAction(setMpvpauseAction);
    //    m_traymenu->addAction(setMpvstopAction);
    m_traymenu->addAction(exitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_traymenu);
    m_trayIcon->setToolTip("双击打开配置界面");
    m_trayIcon->setIcon(QIcon(":/tray.ico"));
    m_trayIcon->show();
    //void activated(QSystemTrayIcon::ActivationReason reason);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, [ = ](QSystemTrayIcon::ActivationReason reason) {
        if (QSystemTrayIcon::Trigger == reason) {
            if (m_parentMainWindow) {
                m_parentMainWindow->show();
                m_parentMainWindow->activateWindow();
            }
        }
    });

    connect(dApp, &Application::pathChanged, this, &settingWindow::pathChanged);


}
void settingWindow::pathChanged(const QString &path)
{
    m_currentPath = path;
    ui->pathEdit->setText(m_currentPath);
}
settingWindow::~settingWindow()
{

    delete ui;
}

void settingWindow::readSettings()
{
    QString path = "/opt/deepin-dreamscene-ui/";
    QSettings settings(path + SETTINGPATH, QSettings::IniFormat);

    m_currentPath = settings.value("WallPaper/CurrentPath").toString();
    m_crrenNumber = settings.value("WallPaper/ScrrenNumber").toInt(); //1-2
    m_isAutoStart = settings.value("WallPaper/isAutoStart").toInt();

    if (m_crrenNumber > 1) {
//        ui->autoisMScreen->setCheckState(Qt::Checked);
    }
    if (m_isAutoStart > 0) {
        ui->autoStartBox->setCheckState(Qt::Checked);
    }

    qDebug() << "x";
}

void settingWindow::saveSettings()
{
    QString path = "/opt/deepin-dreamscene-ui/";
    QSettings settings(path + SETTINGPATH, QSettings::IniFormat);
    settings.clear();
    settings.setValue("WallPaper/ScrrenNumber", m_crrenNumber);
    settings.setValue("WallPaper/isAutoStart", m_isAutoStart);
    settings.setValue("WallPaper/CurrentPath", m_currentPath);

    QSettings settings1(SETTINGPATH, QSettings::IniFormat);
    settings1.clear();
    settings1.setValue("WallPaper/ScrrenNumber", m_crrenNumber);
    settings1.setValue("WallPaper/isAutoStart", m_isAutoStart);
    settings1.setValue("WallPaper/CurrentPath", m_currentPath);

}

QString settingWindow::getCurrentPath()
{
    return m_currentPath;
}

int settingWindow::getCurrentNumber()
{
    return m_crrenNumber;
}

int settingWindow::isAutoStart()
{
    return m_isAutoStart;
}

void settingWindow::cpToTmp()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/deepin-dreamscene/";
    QDir dir(path);
    if (!dir.exists()) {
        QDir dir1;
        dir1.mkpath(path);
    }
    QFile::copy(QCoreApplication::applicationDirPath() + "/dynamicWallPaper.desktop", path + "dynamicWallPaper.desktop");

    QProcess::execute("cp libdde-file-manager.so.1.8.2 " + path);
    QProcess::execute("cp libdde-file-manager.so.1.8 " + path);
    QProcess::execute("cp libdde-file-manager.so.1 " + path);
    QProcess::execute("cp libdde-file-manager.so " + path);
    QProcess::execute("cp dde-desktop " + path);
    QProcess::execute("cp deepin-dreamscene " + path);
    QProcess::execute("cp config.ini " + path);
    QProcess::execute("cp 9.mp4 " + path);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    qDebug() << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
//        qDebug()<<QStandardPaths::HomeLocation;
    qDebug() << QCoreApplication::applicationDirPath() + "/dynamicWallPaper.desktop";
}

void settingWindow::setScreenMode(const QString &arg)
{
    emit dApp->setScreenMode(arg);
}

void settingWindow::on_pathBtn_clicked()
{
    QString path = QFileDialog::getOpenFileName();
    ui->pathEdit->setText(path);
}

void settingWindow::on_setBtn_clicked()
{
    if (ui->pathEdit->text() != nullptr) {
        m_currentPath = ui->pathEdit->text();
        emit dApp->setPlayPath(ui->pathEdit->text());
        emit dApp->setMpvPlay();
        saveSettings();
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
}

void settingWindow::on_stopBtn_clicked()
{
    emit dApp->setMpvstop();
}

void settingWindow::on_Slider_valueChanged(int value)
{
    emit dApp->setMpvVolume(value);
}

void settingWindow::on_startBtn_clicked()
{
    emit dApp->setMpvPlay();
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
    QProcess::execute("cp /opt/deepin-dreamscene-ui/deepin-dreamscene-ui.desktop " + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/");
}

void settingWindow::on_noAutoStart_clicked()
{
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/dynamicWallPaper.desktop");
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
}

void settingWindow::on_comboBox_activated(const QString &arg1)
{
    setScreenMode(arg1);
}
