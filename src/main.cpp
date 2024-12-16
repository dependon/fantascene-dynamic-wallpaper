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
    textStream << txt << endl;

    // 删除10天前的日志文件
    QDir logDir(logFolder);
    QStringList filters;
    filters << "log_*.txt";
    logDir.setNameFilters(filters);

    QFileInfoList fileList = logDir.entryInfoList();
    for (QFileInfo fileInfo: fileList) {
        QDateTime fileTime = fileInfo.created();
        if (fileTime.daysTo(QDateTime::currentDateTime()) > 10) {
            QFile::remove(fileInfo.absoluteFilePath());
        }
    }
}

int main(int argc, char *argv[])
{

#ifdef Q_OS_LINUX
    qputenv("QT_QPA_PLATFORM", "xcb");
    mallopt(M_ARENA_MAX, 1);
#endif
    Application a(argc, argv);
    a.setApplicationVersion("1.0.0");
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


