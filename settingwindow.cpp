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
DCORE_USE_NAMESPACE

#define SETTINGPATH "config.ini"
const QString CONFIG_PATH =   QDir::homePath() +
                              "/.config/deepin-dreamscene-ui/config.ini";
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
    connect(exitAction, &QAction::triggered, this, &settingWindow::quitApp);
    connect(dApp, &Application::quitApp, this, &settingWindow::quitApp, Qt::DirectConnection);

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
    if (!m_currentPath.isEmpty()) {
        ui->pathEdit->setText(m_currentPath);
        QPixmap pix = dApp->getThumbnail(m_currentPath);
        if (!pix.isNull()) {
            ui->pixThumbnail->setPixmap(pix);
        }
    }
}
settingWindow::~settingWindow()
{

    delete ui;
}

void settingWindow::readSettings()
{
    QSettings settings(CONFIG_PATH, QSettings::IniFormat);

    m_currentPath = settings.value("WallPaper/CurrentPath").toString();
    m_crrenNumber = settings.value("WallPaper/ScrrenNumber").toInt(); //1-2
    m_isAutoStart = settings.value("WallPaper/isAutoStart").toInt();
    int widthPY = settings.value("WallPaper/widthPY").toInt();
    int heightPY = settings.value("WallPaper/heightPY").toInt();
    int width = settings.value("WallPaper/width").toInt();
    int height = settings.value("WallPaper/height").toInt();
    m_currentMode = settings.value("WallPaper/Mode").toString();
    m_voiceVolume = settings.value("WallPaper/voiceVolume").toInt();
    dApp->m_manual.setRect(widthPY, heightPY, width, height);
    if (!m_currentMode.isEmpty()) {
        ui->comboBox->setCurrentText(m_currentMode);
        setScreenMode(m_currentMode);
    }
    if (!m_currentPath.isEmpty()) {
        ui->pathEdit->setText(m_currentPath);
        QPixmap pix = dApp->getThumbnail(m_currentPath);
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
    });



    qDebug() << "x";
}

void settingWindow::saveSettings()
{
    QSettings settings(CONFIG_PATH, QSettings::IniFormat);
    settings.clear();
    settings.setValue("WallPaper/ScrrenNumber", m_crrenNumber);
    settings.setValue("WallPaper/isAutoStart", m_isAutoStart);
    settings.setValue("WallPaper/CurrentPath", m_currentPath);
    settings.setValue("WallPaper/Mode", ui->comboBox->currentText());
    settings.setValue("WallPaper/widthPY", dApp->m_manual.x());
    settings.setValue("WallPaper/heightPY", dApp->m_manual.y());
    settings.setValue("WallPaper/width", dApp->m_manual.width());
    settings.setValue("WallPaper/height", dApp->m_manual.height());
    settings.setValue("WallPaper/voiceVolume", m_voiceVolume);
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
    m_voiceVolume = value;
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
    if (!QFileInfo(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/deepin-dreamscene-ui.desktop").isFile())
        QProcess::execute("cp /opt/durapps/deepin-dreamscene-ui/deepin-dreamscene-ui.desktop " + QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/");
}

void settingWindow::on_noAutoStart_clicked()
{
    QFile::remove(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/autostart/deepin-dreamscene-ui.desktop");
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
}

void settingWindow::on_setManual_clicked()
{
    int widthPY = ui->widthPY->text().toInt();
    int heightPY = ui->heightPY->text().toInt();
    int width = ui->width->text().toInt();
    int height = ui->height->text().toInt();
    dApp->m_manual.setRect(widthPY, heightPY, width, height);
    if (ui->comboBox->currentText() == "手动设置尺寸") {
        emit dApp->sigupdateGeometry();
    }
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
#endif
    dApp->exit();
}



void settingWindow::on_mainWeb_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/dependon/deepin-dreamscene-ui")));
}
