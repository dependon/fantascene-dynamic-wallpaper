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
#include <malloc.h>

#include "loadTranslation.h"
#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settingwindow.h"
#ifdef HAVE_LAYER_SHELL_QT
#include "utils/wayland_utils.h"
#endif

#include <QObject>
#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QLockFile>
#include <QDBusInterface>
#include <QDBusPendingCall>

#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QScreen>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>


/* instance lock path */
#define INSTANCE_LOCK_PATH ".cache/fantascene"

/* instance lock name */
#define INSTANCE_LOCK "single"

#include <QHostAddress>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    QString timestampS = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        txt = timestampS + QString(" Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = timestampS + QString(" Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = timestampS + QString(" Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = timestampS + QString(" Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = timestampS + QString(" Fatal: %1").arg(msg);
        break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd");
#ifdef Q_OS_LINUX
    QString logFolder = QDir::homePath() + "/"+INSTANCE_LOCK_PATH +"/log";
    QDir().mkpath(logFolder); // 创建log文件夹

    QString logFileName = QString("%1/log/log_%2.txt").arg(QDir::homePath() + "/"+INSTANCE_LOCK_PATH ).arg(timestamp);
#else
    QString logFolder = "log";
    QDir().mkpath(logFolder); // 创建log文件夹

    QString logFileName = QString("%1/log/log_%2.txt").arg(QCoreApplication::applicationDirPath()).arg(timestamp);
#endif
    QFile outFile(logFileName);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outFile);
#if QT_VERSION_MAJOR == 5
    textStream << txt << endl;
#else
    textStream << txt << Qt::endl;
#endif
    // 删除10天前的日志文件
    QDir logDir(logFolder);
    QStringList filters;
    filters << "log_*.txt";
    logDir.setNameFilters(filters);

    QFileInfoList fileList = logDir.entryInfoList();
    for (QFileInfo fileInfo: fileList) {
#if QT_VERSION_MAJOR == 5
        QDateTime fileTime = fileInfo.created();
#else
        QDateTime fileTime = fileInfo.fileTime(QFile::FileBirthTime);
#endif
        if (fileTime.daysTo(QDateTime::currentDateTime()) > 10) {
            QFile::remove(fileInfo.absoluteFilePath());
        }
    }
}

int main(int argc, char *argv[])
{

#ifdef Q_OS_LINUX
#ifdef HAVE_LAYER_SHELL_QT
    const QByteArray requestedPlatform = qgetenv("QT_QPA_PLATFORM");
    const bool platformAllowsWayland = requestedPlatform.isEmpty()
        || requestedPlatform.contains("wayland");
    const bool waylandSession = platformAllowsWayland
        && (requestedPlatform.startsWith("wayland")
        || qEnvironmentVariable("XDG_SESSION_TYPE") == QLatin1String("wayland")
        || !qEnvironmentVariableIsEmpty("WAYLAND_DISPLAY"));

    // Enforce Wayland platform under Wayland so LayerShell sets correctly.
    if (waylandSession) {
        qputenv("QT_QPA_PLATFORM", "wayland");
    } else if (requestedPlatform.isEmpty()) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
#else
    // Qt 5 builds retain the established X11 backend.
    // While Qt6 builds gain LayerShellQt6 support (I have to pick one from
    // LayerShellQt5 and LayerShellQt6 because those two are conflict on Debian)
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif
    mallopt(M_ARENA_MAX, 1);
#endif
    Application a(argc, argv);
    a.setApplicationVersion("1.0.0");

    bool layerShellUnavailable = false;
#ifdef HAVE_LAYER_SHELL_QT
    if (QGuiApplication::platformName() == QLatin1String("wayland")) {
        Utils::LayerShell::DetectLayerShellSupport();
        layerShellUnavailable = !Utils::LayerShell::IsLayerShellAvailable();
    }
#endif
#ifndef QT_DEBUG
    // 安装自定义的消息处理程序
    qInstallMessageHandler(customMessageHandler);
#endif
    qDebug()<<"start " <<QGuiApplication::platformName();

    qDebug()<< QApplication::applicationDirPath();
    QString transPath = QApplication::applicationDirPath() + "/translations";
    QDir myDir(transPath);
    if(myDir.exists())
    {
        a.load_translation_sys_files(transPath);
    }
    else {
        a.load_translation_sys_files(TRANSALTION_PATH);
    }

    setlocale(LC_NUMERIC, "C");

    /*
     * Check if there are multiple instances
     * If there are multiple instances, exit now.
    */
#ifdef Q_OS_WIN
    const QString lock = QApplication::applicationDirPath() + "/"+INSTANCE_LOCK;
#else
    const QString lock = QDir::homePath() + "/" + INSTANCE_LOCK_PATH + INSTANCE_LOCK;
#endif
    QLockFile lockFile(lock);

    if (!lockFile.tryLock(300))
    {
        qDebug() << "The application is already running!";
        QDBusInterface iface("com.deepin.dde.fantascene",
                             "/com/deepin/dde/fantascene",
                             "com.deepin.dde.fantascene",
                             QDBusConnection::sessionBus());
        iface.asyncCall("activeWindow");
        return 0;
    }

    QMainWindow *mainwindw = new QMainWindow();
    settingWindow *window = new settingWindow(mainwindw, mainwindw);
    mainwindw->setCentralWidget(window);
    mainwindw->setFixedSize(QSize(MAINWINDOWWIDTH, MAINWINDOWHEIGHT));
    mainwindw->setWindowTitle(QObject::tr("fantacy"));
    mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));
    mainwindw->move(QGuiApplication::primaryScreen()->geometry().center() - mainwindw->rect().center());

    if (layerShellUnavailable) {
        QTimer::singleShot(0, mainwindw, [mainwindw]() {
            mainwindw->show();
            QMessageBox::warning(
                mainwindw,
                QObject::tr("Wayland compositor is not supported"),
                QObject::tr("The current Wayland compositor does not provide "
                    "zwlr_layer_shell_v1. Dynamic wallpaper surfaces have been "
                    "disabled to prevent an ordinary fullscreen window from being "
                    "created. GNOME Wayland requires a GNOME Shell extension or "
                    "another compatible backend."));
        });
    }

    QString envName("DDE_SESSION_PROCESS_COOKIE_ID");
    QByteArray cookie = qgetenv(envName.toUtf8().data());
    qunsetenv(envName.toUtf8().data());
    if (cookie.isEmpty()) {
        QDBusInterface iface("com.deepin.SessionManager",
                             "/com/deepin/SessionManager",
                             "com.deepin.SessionManager",
                             QDBusConnection::sessionBus());
        iface.asyncCall("Register", QString(cookie));
    }


    return a.exec();
}
