#include <malloc.h>

#include "instance.h"
#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settingwindow.h"

#include <QObject>
#include <QMainWindow>
#include <QProcess>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>
#include <QCoreApplication>
#include <QFile>
#include <QStandardPaths>

#define TRANSALTIONPATH "/usr/share/fantascene-dynamic-wallpaper/translations"

/* instance lock path */
#define INSTANCE_LOCK_PATH ".cache/deepin/fantascene"

/* instance lock name */
#define INSTANCE_LOCK "single"

void cpToTmp()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.config/deepin-dreamscene/";
    QDir dir(path);
    if (!dir.exists()) {
        QDir dir1;
        dir1.mkpath(path);
    }
}
int main(int argc, char *argv[])
{
    /*
     * Check if there are multiple instances
     * If there are multiple instances, exit now.
    */
    if (!check_instance_status(INSTANCE_LOCK_PATH,INSTANCE_LOCK)) {
        exit (1);
    }

    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);
    a.setApplicationVersion("1.0.0");
#ifdef Q_OS_LINUX
    QDir dir(TRANSALTIONPATH);
    if (dir.exists()) {
        QDirIterator qmIt(TRANSALTIONPATH, QStringList() << QString("*%1.qm").arg(QLocale::system().name()), QDir::Files);
        while (qmIt.hasNext()) {
            qmIt.next();
            QFileInfo finfo = qmIt.fileInfo();
            QTranslator *translator = new QTranslator;
            if (translator->load(finfo.baseName(), finfo.absolutePath())) {
                qApp->installTranslator(translator);
            }
        }
    }
#endif
    setlocale(LC_NUMERIC, "C");

    if (checkOnly()) {
        bool isShowMainWindow = true;

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
//            mainwindw->show();
        }
        mainwindw->setFixedSize(QSize(640, 500));
        mainwindw->setWindowTitle("动态壁纸");
        mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));

        mainwindw->move(qApp->desktop()->screen()->rect().center() - mainwindw->rect().center());

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


    } else {
        QDBusInterface iface("com.deepin.dde.fantascene",
                             "/com/deepin/dde/fantascene",
                             "com.deepin.dde.fantascene",
                             QDBusConnection::sessionBus());
        iface.asyncCall("activeWindow");
        return 0;
    }

    return a.exec();
}


