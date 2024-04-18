#include <QObject>

#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settings.h"
#include <malloc.h>
#include "settingwindow.h"
#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>
#include "loadTranslation.h"

/* Translation file path */
#define TRANSALTION_PATH "/usr/share/fantascene-dynamic-wallpaper/translations"

/* instance lock path */
#define INSTANCE_LOCK_PATH ".cache/fantascene"

/* instance lock name */
#define INSTANCE_LOCK "single"

int main(int argc, char *argv[])
{
#ifdef Q_OS_LINUX
    qputenv("QT_QPA_PLATFORM", "xcb");
    mallopt(M_ARENA_MAX, 1);
#endif
    QString path = "/opt/durapps/fantascene-dynamic-wallpaper/";

    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);
    a.setApplicationVersion("1.0.0");
#ifdef Q_OS_LINUX
    qDebug()<< QApplication::applicationDirPath();
    QString transPath = QApplication::applicationDirPath() + "/translations";
    QDir myDir(transPath);
    if(myDir.exists())
    {
        load_translation_files(transPath);
    }
    else {
        load_translation_files(TRANSALTION_PATH);
    }
#endif
//    a.setTheme("light");
    setlocale(LC_NUMERIC, "C");
    /*
     * Check if there are multiple instances
     * If there are multiple instances, exit now.
    */
    const QString lock = QDir::homePath() + "/" + INSTANCE_LOCK_PATH + INSTANCE_LOCK;
    QLockFile lockFile(lock);

    if (lockFile.tryLock(300)) {
        bool isShowMainWindow = true;
        // 检测 dde-desktop 进程是否在运行中
        QProcess process;
        process.start("pgrep", QStringList() << "-x" << "dde-desktop");
        process.waitForFinished();
        QByteArray result = process.readAllStandardOutput();
        if (result.isEmpty())
        {
            qDebug() << "dde-desktop 未找到，等待五秒后继续检测";
            QThread::sleep(5);
        }
#ifdef DEEPINV20
        isShowMainWindow = false;
        if (QFileInfo(path + "dde-desktop").isFile() && !QFileInfo(path + "dde-desktop").isExecutable()) {
            int iRet = QProcess::execute("pkexec chmod 777 " + path + "dde-desktop " + path + "config.ini");
            if (iRet != 0) {
                return 0;
            }
            isShowMainWindow = true;
        } else {
            qDebug() << "可以启动: " << path + "dde-desktop";
        }
#endif


        dApp->m_startDesktop  = QThread::create([ = ]() {
#ifdef MY_V23SUPER
            //打印当前路径
            qDebug() << "打印当前路径1";
            qDebug() << QCoreApplication::applicationDirPath();
            qDebug() << "打印当前路径2";
            QStringList arguments;
            arguments << "--set" << "-a" << "org.deepin.dde.file-manager" << "-r" << "org.deepin.dde.file-manager.plugins" << "-k" << "desktop.blackList" << "-v" << "[\"ddplugin-background\"]";

            QProcess process;
            process.start("dde-dconfig", arguments);
            process.waitForFinished(-1);
            QProcess::execute("killall dde-desktop");

            QProcess pro;
            QString strPath = QString("dde-desktop");
            qDebug()<<"yuansheng qi dong ";
            pro.startDetached(strPath);
#endif



#ifdef DEEPINV20
            QProcess process;
            process.start("dde-dconfig", arguments);
            process.waitForFinished(-1);
            QProcess::execute("killall dde-desktop");
            QProcess pro;
            QString strPath = path + QString("dde-desktop");
            pro.startDetached(strPath);
#endif

            qDebug() << "启动失败: " << path + "dde-desktop";
        });
        dApp->m_startDesktop->start();

        QTimer::singleShot(1000, [ = ] {
            QMainWindow *mainwindw = new QMainWindow();
            settingWindow *window = new settingWindow(mainwindw, mainwindw);
            mainwindw->setCentralWidget(window);
            int index = 0;
            for (const QString &arg : qApp->arguments()) {
                if (arg == "min") {
                    index++;
                }
            }
            if (index == 0 && isShowMainWindow) {
                //mainwindw->show();
            }
            mainwindw->setFixedSize(QSize(640, 500));
            mainwindw->setWindowTitle("动态壁纸");
            mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));
            // mainwindw->titlebar()->setIcon(QIcon(":/install/wallpaper.png"));
            // mainwindw->titlebar()->setWindowTitle("动态壁纸");

            mainwindw->move(qApp->desktop()->screen()->rect().center() - mainwindw->rect().center());

            //Wallpaper *w = new Wallpaper(window->getCurrentPath(), window->getCurrentNumber());
            dApp->setDesktopTransparent();
            QTimer::singleShot(1000, [ = ] {
                dApp->setDesktopTransparent();
            });
    //        qDebug() << qApp->desktop()->screenNumber();
    //        qDebug() << qApp->desktop()->primaryScreen();
    //        qDebug() << qApp->desktop()->screenCount();

            //DBusWallpaperService *dbusInter = new DBusWallpaperService(w);
    //        Q_UNUSED(dbusInter);

            QDBusConnection::sessionBus().registerService("com.deepin.dde.fantascene");
            //QDBusConnection::sessionBus().registerObject("/com/deepin/dde/fantascene", "com.deepin.dde.fantascene", w);

            QString envName("DDE_SESSION_PROCESS_COOKIE_ID");

            QByteArray cookie = qgetenv(envName.toUtf8().data());
            qunsetenv(envName.toUtf8().data());

            if (!cookie.isEmpty()) {
                QDBusInterface iface("com.deepin.SessionManager",
                                     "/com/deepin/SessionManager",
                                     "com.deepin.SessionManager",
                                     QDBusConnection::sessionBus());
                iface.asyncCall("Register", QString(cookie));
            }

        });

    } else {
        QDBusInterface iface("com.deepin.dde.fantascene",
                             "/com/deepin/dde/fantascene",
                             "com.deepin.dde.fantascene",
                             QDBusConnection::sessionBus());
        iface.asyncCall("activeWindow");
        a.m_bexitDesktop = false;
        return 0;
    }

    return a.exec();
}


