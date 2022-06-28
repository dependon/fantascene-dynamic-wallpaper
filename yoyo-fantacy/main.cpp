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
//#include <DDesktopEntry>
#include <QFile>
//#include <DTitlebar>
#include <QStandardPaths>

/* Translation file path */
#define TRANSALTION_PATH "/usr/share/yoyo-fantacy/translations"

/* instance lock path */
#define INSTANCE_LOCK_PATH ".cache/yoyo-fantacy"

/* instance lock name */
#define INSTANCE_LOCK "single"

void load_translation_files(const QString path)
{
    QDir dir(path);
    if (dir.exists())
    {
        QDirIterator qmIt(path, QStringList() << QString("*%1.qm").arg(QLocale::system().name()), QDir::Files);
        while (qmIt.hasNext())
        {
            qmIt.next();
            QFileInfo finfo = qmIt.fileInfo();
            QString tr_path = finfo.absolutePath();
            QString tr_file = finfo.baseName();
            QTranslator *translator = new QTranslator;

            if (!translator->load(tr_file,tr_path)) {
                qWarning() << "Failed to load " + tr_path, + "/" + tr_file + " ,fallback to default!";
            }
            if (!qApp->installTranslator(translator)) {
                qWarning() << "Failed to install translation!";
            }
        }
    }
}

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
    cpToTmp();
    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);
//    a.loadTranslator();
//    a.setApplicationVersion(QApplication::buildVersion("1.0.0"));
//    a.setTheme("light");
    setlocale(LC_NUMERIC, "C");
#ifdef Q_OS_LINUX
    load_translation_files(TRANSALTION_PATH);
#endif

    /*
     * Check if there are multiple instances
     * If there are multiple instances, exit now.
    */
    const QString lock = QDir::homePath() + "/" + INSTANCE_LOCK_PATH + INSTANCE_LOCK;
    QLockFile lockFile(lock);
    if (!lockFile.tryLock(300)){
        qDebug() << "The application is already running!";
        QDBusInterface iface("com.deepin.dde.fantascene",
                             "/com/deepin/dde/fantascene",
                             "com.deepin.dde.fantascene",
                             QDBusConnection::sessionBus());
        iface.asyncCall("activeWindow");
        return 0;
    }
    bool isShowMainWindow = true;

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
        if (index == 0 ) {
            mainwindw->show();
        }
        mainwindw->setFixedSize(QSize(640, 500));
        mainwindw->setWindowTitle("动态壁纸");
        mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));

        mainwindw->move(qApp->desktop()->screen()->rect().center() - mainwindw->rect().center());

        Wallpaper *w = new Wallpaper(window->getCurrentPath(), window->getCurrentNumber());
        dApp->setDesktopTransparent();
        QTimer::singleShot(1000, [ = ] {
            dApp->setDesktopTransparent();
        });
        //        qDebug() << qApp->desktop()->screenNumber();
        //        qDebug() << qApp->desktop()->primaryScreen();
        //        qDebug() << qApp->desktop()->screenCount();

        DBusWallpaperService *dbusInter = new DBusWallpaperService(w);
        //        Q_UNUSED(dbusInter);

        QDBusConnection::sessionBus().registerService("com.deepin.dde.fantascene");
        QDBusConnection::sessionBus().registerObject("/com/deepin/dde/fantascene", "com.deepin.dde.fantascene", w);

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



    return a.exec();
}


