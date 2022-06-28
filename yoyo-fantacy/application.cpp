#include "application.h"
#include <QIcon>
#include <QDir>
#include <QUrl>
#include <QMutexLocker>
#include <QWindow>
#include <QCryptographicHash>
#include <QTimer>
#include <QDBusInterface>
#include <QDebug>

#include "setdesktop.h"

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
    this->setApplicationName(tr("yoyo-fantacy"));
    this->setApplicationDisplayName(tr("yoyo-fantacy"));

    this->setWindowIcon(QIcon(":/install/wallpaper.png"));


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
}

Application::~Application()
{
    emit quitApp();
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
//    return false;
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
//    QProcess::execute(commod);
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
//    const QString failEncodePath = cacheP + "/fail/" + md5s + ".png";

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
        return a;
    }

    return QPixmap();
}

void Application::setDesktopTransparent()
{
    //dbus开启壁纸透明
//    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground false");
    QDBusInterface iface("com.yoyo.Settings",
                         "/Theme",
                         "com.yoyo.Theme",
                         QDBusConnection::sessionBus());
    iface.asyncCall("setBackgroundVisible", false);
    //设置desktop透明
    char str[13] = "yoyo-desktop";
    int pid_t[128];
    find_pid_by_name1(str, pid_t);
    int pid = pid_t[0];
    Display *display = XOpenDisplay(0);
    WindowsMatchingPid match(display, XDefaultRootWindow(display), pid);
    const list<Window> &result = match.result();
    for (Window id : result) {
        QWindow *window = QWindow::fromWinId((unsigned long)id);
        if (window != nullptr) {
            window->setOpacity(0.99);
        }
        if (!m_screenWid.contains(id)) {
            m_screenWid.push_back(id);
        }
    }
}
