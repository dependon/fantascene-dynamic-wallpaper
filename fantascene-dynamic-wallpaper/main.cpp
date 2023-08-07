#include <QObject>

#include "application.h"
#include "wallpaper.h"
#include "dbuswallpaperservice.h"
#include "settings.h"
#include <malloc.h>
#include "settingwindow.h"
#include <DMainWindow>
#include <QProcess>
#include <QThread>
#include <QDesktopWidget>
#include <QDebug>
#include <QCoreApplication>
#include <DDesktopEntry>
#include <QFile>
#include <DTitlebar>
#include <QStandardPaths>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

/* Translation file path */
#define TRANSALTION_PATH "/usr/share/fantascene-dynamic-wallpaper/translations"

#ifdef Q_OS_LINUX
/*
 * Load translation files
 *
 * @ Accepts parameters are : path (const)
 *
 * @ Returns no results
 */
void load_translation_files(const QString path)
{
    QDir dir(path);
    qDebug()<<"event: "<<QLocale::system().name();
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
#endif

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
    qputenv("QT_QPA_PLATFORM", "xcb");
    cpToTmp();
    QString path = "/opt/apps/com.deepin.fantacy/files/bin/";

    mallopt(M_ARENA_MAX, 1);

    Application a(argc, argv);
    a.loadTranslator();
    a.setApplicationVersion(DApplication::buildVersion("1.0.0"));
//    a.setTheme("light");
    setlocale(LC_NUMERIC, "C");

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

    if (a.setSingleInstance("fantascene-dynamic-wallpaper")) {
        bool isShowMainWindow = true;
#if 1
        isShowMainWindow = false;

        dApp->m_startDesktop  = QThread::create([ = ]() {
            //打印当前路径
            qDebug() << "打印当前路径1";
            qDebug() << QCoreApplication::applicationDirPath();
            qDebug() << "打印当前路径2";

            QString desktopPath = "bash " + QApplication::applicationDirPath() + "/startdesktop.sh";
            QProcess pro;
            if (QFileInfo(QApplication::applicationDirPath() + "/startdesktop.sh").isFile())
            {
                pro.startDetached("killall dde-desktop");
                pro.startDetached(QApplication::applicationDirPath() + "/dde-desktop");
                qDebug() << "启动失败: " << QApplication::applicationDirPath() + "/dde-desktop";
            }
            else {
                QString strPath = "bash /opt/apps/com.deepin.fantacy/files/bin/startdesktop.sh";
                pro.startDetached(strPath);
                qDebug() << "启动失败: " << path + "dde-desktop";
            }

        });
        dApp->m_startDesktop->start();
#endif

        QTimer::singleShot(1000, [ = ] {
            DMainWindow *mainwindw = new DMainWindow();
            settingWindow *window = new settingWindow(mainwindw, mainwindw);
            mainwindw->setCentralWidget(window);
            int index = 0;
            for (const QString &arg : qApp->arguments()) {
                if (arg == "min") {
                    index++;
                }
            }
            if (index == 0 && isShowMainWindow) {
                mainwindw->show();
            }
            mainwindw->setFixedSize(QSize(640, 500));
            mainwindw->setWindowTitle("动态壁纸");
            mainwindw->setWindowIcon(QIcon(":/install/wallpaper.png"));
            mainwindw->titlebar()->setIcon(QIcon(":/install/wallpaper.png"));
            mainwindw->titlebar()->setWindowTitle("动态壁纸");

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


