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


#include "application.h"

#include <QIcon>
#include <QUrl>
#include <QMutexLocker>
#include <QWindow>
#include <QDebug>
#include <QCryptographicHash>
#include <QTimer>
#include <QPainter>
#include <QProcess>
#include <QDateTime>
#include <QScreen>
#include <QDBusInterface>
#include <QDBusPendingCall>
#include <QStyleFactory>
#include <QPalette>

#include "db/dbmanager.h"

#include "setdesktop.h"

#include <X11/Xlib.h>
#include <xcb/xcb.h>

Display *display = nullptr;
xcb_connection_t *connection = nullptr;
int find_pid_by_name1(char *ProcName, int *foundpid)
{
    DIR             *dir;
    struct dirent   *d;
    int             pid, i;
    char            *s;
    int pnlen;

    i = 0;
    foundpid[0] = 0;
    pnlen = strlen(ProcName);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir) {
        printf("cannot open /proc");
        return -1;
    }

    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {

        char exe [PATH_MAX + 1];
        char path[PATH_MAX + 1];
        int len;
        int namelen;

        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0)       continue;

        snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
        if ((len = readlink(exe, path, PATH_MAX)) < 0)
            continue;
        path[len] = '\0';

        /* Find ProcName */
        s = strrchr(path, '/');
        if (s == NULL) continue;
        s++;

        /* we don't need small name len */
        namelen = strlen(s);
        if (namelen < pnlen)     continue;

        if (!strncmp(ProcName, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if (s[pnlen] == ' ' || s[pnlen] == '\0') {
                foundpid[i] = pid;
                i++;
            }
        }
    }

    foundpid[i] = 0;
    closedir(dir);

    return  0;

}

const QString toMd5(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    //设置界面风格，qt默认存在Fusion Windows等
//    QStringList styleKeys = QStyleFactory::keys();
//    qDebug() << "Available styles:";
//    for ( QString key : styleKeys) {
//        qDebug() << key;
//    }
//    QString styleName = "Fusion";

//    // 检查该样式是否存在
//    if (QStyleFactory::keys().contains(styleName)) {
//        // 设置应用程序的样式
//        qApp->setStyle(QStyleFactory::create(styleName));
//    } else {
//        qDebug() << "Style" << styleName << "is not available.";
//    }

    // 从外部文件加载QSS样式表
    QFile styleFile(":/style/MainQss.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理文件无法打开的错误
        qDebug() << "无法打开样式表文件";
        QTextStream styleStream(&styleFile);
        QString styleSheet = styleStream.readAll();
        styleFile.close();
        qApp->setStyleSheet(styleSheet);
    }


    this->setApplicationName(tr("fantascene-dynamic-wallpaper"));
    this->setApplicationDisplayName(tr("fantascene-dynamic-wallpaper"));
//    this->setApplicationDescription(
//        QObject::tr(
//            "<span style='font-size:10pt;font-weight:60;'>wallpaper by deepin community</span><br/>"
//            "<a href='https://github.com/dependon/fantascene-dynamic-wallpaper/'>github/fantascene-dynamic-wallpaper</a><br/>"
//            "<a href='https://gitee.com/liuminghang/fantascene-dynamic-wallpaper/'>gitee/fantascene-dynamic-wallpaper</a><br/>"
//            "<span style='font-size:12pt;'>mail: liuminghang0821@gmail.com</span><br/><br/>"
//            "Published under GPL V3"
//        )
//    );
//    this->setProductIcon(QIcon(":/install/wallpaper.png"));
    this->setWindowIcon(QIcon(":/install/wallpaper.png"));

    m_currentScreenNum = screens().size();
    m_pplaylistTimer = new QTimer(this);
    connect(m_pplaylistTimer, &QTimer::timeout, this, [ = ] {
        if (m_isPlayList && m_playlistPath.contains(m_currentPath))
        {
            int index = m_playlistPath.indexOf(m_currentPath);
            if (index < m_playlistPath.count() - 1) {
                setWallPaper(m_playlistPath[index + 1]);
            } else {
                setWallPaper(m_playlistPath[0]);
            }
        }
    });
    m_pplaylistTimer->start(1000 * m_PlaylistTimer);

    setQuitOnLastWindowClosed(false);

}

Application::~Application()
{
    if(display)
    {
        XCloseDisplay(display);
        display = nullptr;
    }
    if(connection)
    {
        xcb_disconnect(connection);
        connection = nullptr;
    }
    Q_EMIT quitApp();
}

const QString Application::thumbnailCachePath()
{
    QString cacheP;

    QStringList systemEnvs = QProcess::systemEnvironment();
    for (QString it : systemEnvs) {
        QStringList el = it.split("=");
        if (el.length() == 2 && el.first() == "XDG_CACHE_HOME") {
            cacheP = el.last();
            break;
        }
    }
    cacheP = cacheP.isEmpty() ? (QDir::homePath() + "/.cache") : cacheP;

    // Check specific size dir
    const QString thumbCacheP = cacheP + "/thumbnails";
    QDir().mkpath(thumbCacheP + "/normal");
    QDir().mkpath(thumbCacheP + "/large");
    QDir().mkpath(thumbCacheP + "/fail");

    return thumbCacheP;
}

bool Application::setThumbnail(const QString &path)
{
    QUrl url;
    if (!path.contains("file://")) {
        url = QUrl::fromLocalFile(path);
    } else {
        url = QUrl(path);
    }
    const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
    const QString thumPath = PIC_DIR_PATH + "/" + md5s + ".png";
    QString commod = "ffmpeg -i " + path + " -ss 00:00:00.000 -vframes 1 -vf 'scale=256:144' " + thumPath + " |y";
    qDebug() << commod;
    if (!QFileInfo(thumPath).isFile()) {
        system(commod.toStdString().c_str());
    }

    return true;
}

void Application::setPlayListTimer(int s)
{
    m_PlaylistTimer = s; //s
    if (m_PlaylistTimer < 10) {
        m_PlaylistTimer = 600;
    }
    m_pplaylistTimer->start(1000 * m_PlaylistTimer);
}

void Application::setisPlayList(bool bRet)
{
    m_isPlayList = bRet;
}

void Application::setSpecialDesktop()
{
    if(!dApp->m_moreData.isShowDesktopIcon && !dApp->m_moreData.isTop )
    {
        if(dApp->m_isUKUI)
        {
           if(QFile::exists("/usr/share/fantascene-dynamic-wallpaper/normal/touming.png"))
           {
               QString command1 = "gsettings set org.mate.background picture-filename 'usr/share/fantascene-dynamic-wallpaper/normal/touming.png'";
               QProcess process1;
               process1.start(command1);
               process1.waitForFinished(-1);  // 等待进程执行完成
           }
           else
           {
               QString command1 = "gsettings set org.mate.background picture-filename ''";
               QProcess process1;
               process1.start(command1);
               process1.waitForFinished(-1);  // 等待进程执行完成
               QString command2 = "gsettings set org.mate.background picture-filename 'none'";
               QProcess process2;
               process2.start(command2);
               process2.waitForFinished(-1);  // 等待进程执行完成
           }
        }
        else if(dApp->m_isGXDE)
        {
            QDBusInterface iface("com.deepin.dde.desktop",
                                 "/com/deepin/dde/desktop",
                                 "com.deepin.dde.desktop",
                                 QDBusConnection::sessionBus());
            iface.asyncCall("EnableBackground", false);
            //设置desktop透明
            char str[12] = "dde-desktop";
            int pid_t[128];
            find_pid_by_name1(str, pid_t);
            int pid = pid_t[0];
            Display *display = XOpenDisplay(0);
            X11MatchingPid match( pid);
            const std::list<Window> &result = match.result();
            for (Window id : result) {
                QWindow *window = QWindow::fromWinId((unsigned long)id);
                if (window != nullptr) {
                    window->setOpacity(0.99);
                    window->raise();
                }
                if (!m_screenWid.contains(id)) {
                    m_screenWid.push_back(id);
                }
            }

            QTimer::singleShot(500, []() {
                system("deepin-kwin --replace &");
            });

        }
        else if(dApp->m_isLingMo)
        {
            QDBusInterface iface("com.lingmo.Settings",
                                 "/Theme",
                                 "com.lingmo.Theme",
                                 QDBusConnection::sessionBus());
            iface.asyncCall("setBackgroundVisible", false);
        }
        else if(dApp->m_isDDE23)
        {
            // 检测 dde-desktop 进程是否在运行中
            bool findDDe = false;
            int findCount = 0;
            while(!findDDe)
            {
                QProcess process;
                process.start("pgrep", QStringList() << "-x" << "dde-desktop");
                process.waitForFinished();
                QByteArray result = process.readAllStandardOutput();

                if (result.isEmpty())
                {
                    qDebug() << "dde-desktop no find ,wait 1s continue find";
                    QThread::sleep(1);
                    findCount++;
                }
                else
                {
                    findDDe = true;
                }
                if(findCount > 5 )
                {
                    findDDe = true;
                }
            }
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
            dApp->m_startDesktop  = QThread::create([ = ]()
            {
                QStringList argumentsGet;
                argumentsGet << "--get" << "-a" << "org.deepin.dde.file-manager" << "-r" << "org.deepin.dde.file-manager.plugins" << "-k" << "desktop.blackList";
                QProcess processGet;
                processGet.start("dde-dconfig", argumentsGet);
                if(processGet.waitForFinished(-1))
                {
                    QByteArray result = processGet.readAllStandardOutput();
                    QString output(result);
                    if(output.contains( "desktop.blackList"))
                    {
                        return ;
                    }
                    QByteArray resultError = processGet.readAllStandardError();
                    QString outputError(resultError);
                    if(outputError.contains( "desktop.blackList"))
                    {
                        return ;
                    }

                }

                QStringList arguments;
                arguments << "--set" << "-a" << "org.deepin.dde.file-manager" << "-r" << "org.deepin.dde.file-manager.plugins" << "-k" << "desktop.blackList" << "-v" << "[\"ddplugin-background\"]";

                QProcess process;
                process.start("dde-dconfig", arguments);
                process.waitForFinished(-1);
                QProcess::execute("killall dde-desktop");

                QProcess pro;
                QString strPath = QString("dde-desktop");
                qDebug()<<"dde no background start!";
                pro.startDetached(strPath);
            });
            dApp->m_startDesktop->start();
#endif
        }
    }
    else
    {
        //kylin os quit!
        if(dApp->m_isUKUI)
        {
           QString command1 = "gsettings set org.mate.background picture-filename ''";
           QProcess process1;
           process1.start(command1);
           process1.waitForFinished(-1);  // 等待进程执行完成
        }
    }
}

const QPixmap Application::getThumbnailText(const QString &path)
{
    QString text;
    if (path.isEmpty()) {
        text = "ERROR!!";
    } else if (QFileInfo(path).isFile()) {
        text = QFileInfo(path).completeBaseName();
    } else {
        text = path;
    }
    // 创建一个 256x144 的 QImage，白色底色
    QImage image(256, 144, QImage::Format_RGB32);
    image.fill(Qt::white);

    QPainter painter(&image); //为这个QImage构造一个QPainter
    QFont font("Arial", 20); // 字体设置为 Arial，大小为 20
    painter.setFont(font);
    painter.setPen(QColor(255, 0, 0)); // 红色笔刷
    painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿

    QRect textRect = QRect(0, 0, 256, 144); // 图像的整个区域
    int flags = Qt::AlignCenter | Qt::TextWordWrap; // 设置文字对齐方式并开启自动换行
    painter.drawText(textRect, flags, text);

    return QPixmap::fromImage(image);
}

bool Application::addLocalPaths(QStringList strList)
{
    bool bRet =false;
    QList< WallpaperData >listData;
    for(QString path : strList)
    {
        if(!m_allPath.contains(path))
        {
            WallpaperData data;
            data.path = path;
            data.name = QFileInfo(path).completeBaseName();
            listData << data;
            m_allPath.push_back(path);
        }
    }
    if(listData.size()>0)
    {
        bRet = DBManager::instance()->addDatas(listData);
    }
    return bRet;

}

bool Application::removeLocalPaths(QStringList strList)
{
    for(QString str : strList)
    {
        m_allPath.removeOne(str);
    }
    return DBManager::instance()->deleteLocalPaths(strList);
}

bool Application::clearLocalPaths()
{
    m_allPath.clear();
    return DBManager::instance()->clearLocalPaths();
}

bool Application::addPlayListaths(QStringList strList)
{
    QStringList  addlist;
    for (QString str : strList)
    {
        if(!m_playlistPath.contains(str))
        {
            m_playlistPath.push_back(str);
            addlist << str;
        }
    }
    return DBManager::instance()->addPlayList(addlist);
}

bool Application::removePlayListPaths(QStringList strList)
{
    for(QString str : strList)
    {
        m_playlistPath.removeOne(str);
    }
    return DBManager::instance()->deletePlayList(strList);
}

bool Application::clearPlayListPaths()
{
    m_playlistPath.clear();
    return DBManager::instance()->clearPlayList();
}


void Application::CheckSystem()
{
    std::vector<WindowInfo>  list = getAllDesktopWindows();
    if(list.size()>0)
    {
        for(WindowInfo info : list)
        {
            if(m_currentDesktopName.length() == 0)
            {
                m_currentDesktopName = info.name.c_str();
            }
            else if ( m_currentDesktopName.length() < info.name.length())
            {
                m_currentDesktopName = info.name.c_str();
            }
            QWindow *window = QWindow::fromWinId(info.wid);
            if (window != nullptr) {
                window->setOpacity(0.99);
            }
            if (!m_screenDesktopWid.contains(info.wid)) {
                m_screenDesktopWid.push_back(info.wid);
            }
        }
    }
    else
    {
        qDebug()<< QDateTime::currentMSecsSinceEpoch();
        QStringList DesktopList;
        DesktopList << "peony-qt-desktop";
        DesktopList << "dde-desktop";
        DesktopList << "nautilus-desktop";
        DesktopList << "yoyo-desktop";
        DesktopList << "cutefish-desktop";
        DesktopList << "plasmashell";
        DesktopList << "pcmanfm-qt";
        DesktopList << "caja";
        DesktopList << "xfdesktop";
        for(QString desktopStr : DesktopList)
        {
            int pid_t[128];
            find_pid_by_name1(desktopStr.toLatin1().data(), pid_t);
            int pid = pid_t[0];
            if(pid > 0)
            {
                if(desktopStr.contains("nautilus-desktop"))
                {
                    //存在,使用默认桌面,初始化参数改变,如果后续修改,依配置文件为准
                    m_moreData.isShowDesktopIcon = false ;
                    m_moreData.isTop = false ;
                }
                m_currentDesktopName = desktopStr;

                X11MatchingPid match( pid);
                const std::list<unsigned long> &result = match.result();
                for (unsigned long id : result) {
                    QWindow *window = QWindow::fromWinId(id);
                    if (window != nullptr) {
                        window->setOpacity(0.99);
                    }
                    if (!m_screenDesktopWid.contains(id)) {
                        m_screenDesktopWid.push_back(id);
                    }
                }
            }
        }
        qDebug()<< QDateTime::currentMSecsSinceEpoch();
    }
}

void Application::changePidOpacity( const double &opacity)
{
    if(m_screenDesktopWid.size()>0)
    {
        m_moreData.m_DesktopTransparency = opacity;
        for (unsigned long id : m_screenDesktopWid) {
            QWindow *window = QWindow::fromWinId(id);
            if (window != nullptr) {
                window->setOpacity(opacity);
            }
        }
        return ;
    }
}

void Application::changeMeOpacity(const double &opacity)
{
    if(m_screenDesktopWid.size()>0)
    {
        m_moreData.m_WallpaperTransparency = opacity;
        for (unsigned long id : m_screenWid) {
            QWindow *window = QWindow::fromWinId(id);
            if (window != nullptr) {
                window->setOpacity(opacity);
            }
        }
        return ;
    }

}

QSet<QString> Application::convertQStringListToSet(const QStringList &list) {
    QSet<QString> set;
    for (const QString &str: list) {
        set.insert(str);
    }
    return set;
}

xcb_connection_t *Application::getXcb_connection_t()
{
    if(!connection)
    {
        connection = xcb_connect(NULL, NULL);
    }
    return connection;
}

void *Application::getDisplay()
{
    if(!display)
    {
        display = XOpenDisplay(NULL);
    }
    return display;
}

void Application::setAppColor(const QString &strColor)
{
    // 设置文本颜色为红色
    QPalette palette = qApp->palette();

    if(strColor.contains("default"))
    {
        palette.setColor(QPalette::Text, Qt::darkGray);
    }
    else if (strColor.contains("white"))
    {
        palette.setColor(QPalette::Text, Qt::white);
    }
    else if (strColor.contains("black"))
    {
        palette.setColor(QPalette::Text, Qt::black);
    }
    else if (strColor.contains("darkGray"))
    {
        palette.setColor(QPalette::Text, Qt::darkGray);
    }
    else if (strColor.contains("gray"))
    {
        palette.setColor(QPalette::Text, Qt::gray);
    }
    else if (strColor.contains("lightGray"))
    {
        palette.setColor(QPalette::Text, Qt::lightGray);
    }
    else if (strColor.contains("red"))
    {
        palette.setColor(QPalette::Text, Qt::red);
    }
    else if (strColor.contains("green"))
    {
        palette.setColor(QPalette::Text, Qt::green);
    }
    else if (strColor.contains("blue"))
    {
        palette.setColor(QPalette::Text, Qt::blue);
    }
    else if (strColor.contains("cyan"))
    {
        palette.setColor(QPalette::Text, Qt::cyan);
    }
    else if (strColor.contains("magenta"))
    {
        palette.setColor(QPalette::Text, Qt::magenta);
    }
    else if (strColor.contains("yellow"))
    {
        palette.setColor(QPalette::Text, Qt::yellow);
    }
    else if (strColor.contains("darkRed"))
    {
        palette.setColor(QPalette::Text, Qt::darkRed);
    }
    else if (strColor.contains("darkGreen"))
    {
        palette.setColor(QPalette::Text, Qt::darkGreen);
    }
    else if (strColor.contains("darkBlue"))
    {
        palette.setColor(QPalette::Text, Qt::darkBlue);
    }
    else if (strColor.contains("darkCyan"))
    {
        palette.setColor(QPalette::Text, Qt::darkCyan);
    }
    else if (strColor.contains("darkMagenta"))
    {
        palette.setColor(QPalette::Text, Qt::darkMagenta);
    }
    else if (strColor.contains("darkYellow"))
    {
        palette.setColor(QPalette::Text, Qt::darkYellow);
    }
    else if (strColor.contains("transparent"))
    {
        palette.setColor(QPalette::Text, QColor(0, 0, 0, 0));
    }

    qApp->setPalette(palette);
}

const QPixmap Application::getThumbnail(const QString &path)
{
    QMutexLocker locker(&mutex);
    QFileInfo info(path);
    //mkdir PIC_DIR_PATH
    QDir dir(PIC_DIR_PATH);
    if (!dir.exists()) {
        QDir dir;
        dir.mkdir(PIC_DIR_PATH);
    }

    const QString cacheP = thumbnailCachePath();
    QUrl url;
    if (!path.contains("file://")) {
        url = QUrl::fromLocalFile(path);
    } else {
        url = QUrl(path);
    }

    QPixmap  wallPix(QFileInfo(path).path() + "/" + "preview.jpg");
    if (!wallPix.isNull()) {
        return wallPix.scaled(256, 144);
    }
    const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
    const QString encodePath = cacheP + "/large/" + md5s + ".png";

    const QString thumPath = PIC_DIR_PATH + "/" + md5s + ".png";
    if (QFileInfo(thumPath).exists()) {
        QPixmap  a(thumPath);
        if (!a.isNull()) {
            return  a;
        }
    }
    if (QFileInfo(encodePath).exists()) {
        QPixmap  a(encodePath);
        if (!a.isNull()) {
            a.save(thumPath);
            return  a;
        }

    }
    if (QFileInfo(path).exists()) {
        qDebug() << "Fail-thumbnail exist, won't regenerate: " ;
        setThumbnail(path);
        QPixmap  a(thumPath);
        if (a.isNull()) {
            a = getThumbnailText(path);
            a.save(thumPath);
        }
        return a;
    }
    QPixmap  tmpA = getThumbnailText(path);
    tmpA.save(thumPath);
    return tmpA;
}

const QPixmap Application::reloadThumbnail(const QString &path)
{
    QFileInfo info(path);
    //mkdir PIC_DIR_PATH
    QDir dir(PIC_DIR_PATH);
    if (!dir.exists()) {
        QDir dir;
        dir.mkdir(PIC_DIR_PATH);
    }

    const QString cacheP = thumbnailCachePath();
    QUrl url;
    if (!path.contains("file://")) {
        url = QUrl::fromLocalFile(path);
    } else {
        url = QUrl(path);
    }
    const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
    const QString thumPath = PIC_DIR_PATH + "/" + md5s + ".png";
    QFileInfo thumbInfo(thumPath);
    if (thumbInfo.exists())
    {
        QFile::remove(thumPath);
    }
    return getThumbnail(path);
}

const QPixmap Application::setNewThumbnail(const QString &path, const QString &thumbnailPath)
{
    QMutexLocker locker(&mutex);
    QPixmap pix(thumbnailPath);
    QPixmap pixRe;
    if (!pix.isNull())
    {
        pixRe = pix.scaled(256, 144);
        QUrl url;
        if (!path.contains("file://")) {
            url = QUrl::fromLocalFile(path);
        } else {
            url = QUrl(path);
        }
        const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
        const QString thumPath = PIC_DIR_PATH + "/" + md5s + ".png";
        QFileInfo thumbInfo(thumPath);
        if (thumbInfo.exists())
        {
            QFile::remove(thumPath);
        }
        pixRe.save(thumPath);

    }
    return pixRe;
}

QString Application::getThumbnailPath(const QString &path)
{
    QMutexLocker locker(&mutex);
    QFileInfo info(path);
    //mkdir PIC_DIR_PATH
    QDir dir(PIC_DIR_PATH);
    if (!dir.exists()) {
        QDir dir;
        dir.mkdir(PIC_DIR_PATH);
    }

    const QString cacheP = thumbnailCachePath();
    QUrl url;
    if (!path.contains("file://")) {
        url = QUrl::fromLocalFile(path);
    } else {
        url = QUrl(path);
    }

    QPixmap  wallPix(QFileInfo(path).path() + "/" + "preview.jpg");
    if (!wallPix.isNull()) {
        return QFileInfo(path).path() + "/" + "preview.jpg";
    }
    else
    {
        const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
        const QString thumPath = PIC_DIR_PATH + "/" + md5s + ".png";
        return  thumPath;
    }
}

void Application::setDesktopTransparent()
{
//    //dbus开启壁纸透明
//    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground false");
//    //设置desktop透明
//    char str[12] = "dde-desktop";
//    int pid_t[128];
//    find_pid_by_name1(str, pid_t);
//    int pid = pid_t[0];
//    Display *display = XOpenDisplay(0);
//    WindowsMatchingPid match(display, XDefaultRootWindow(display), pid);
//    const list<Window> &result = match.result();
//    for (Window id : result) {
//        QWindow *window = QWindow::fromWinId((unsigned long)id);
//        if (window != nullptr) {
//            window->setOpacity(0.99);
//        }
//        if (!m_screenWid.contains(id)) {
//            m_screenWid.push_back(id);
//        }
//    }
}
