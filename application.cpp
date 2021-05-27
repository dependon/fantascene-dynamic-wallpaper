#include "application.h"
#include <QIcon>
#include <QDir>
#include <QUrl>
#include <QMutexLocker>
#include <QWindow>
#include <QCryptographicHash>

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
    : DApplication(argc, argv)
{
    this->setApplicationName(tr("fantascene-dynamic-wallpaper"));
    this->setApplicationDisplayName(tr("fantascene-dynamic-wallpaper"));
    this->setApplicationDescription(
        QObject::tr(
            "<span style='font-size:10pt;font-weight:60;'>wallpaper by deepin community</span><br/>"
            "<a href='https://github.com/dependon/fantascene-dynamic-wallpaper/'>github/fantascene-dynamic-wallpaper</a><br/>"
            "<a href='https://gitee.com/liuminghang/fantascene-dynamic-wallpaper/'>gitee/fantascene-dynamic-wallpaper</a><br/>"
            "<span style='font-size:12pt;'>mail: liuminghang0821@gmail.com</span><br/><br/>"
            "Published under GPL V3"
        )
    );

    this->setProductIcon(QIcon(":/install/wallpaper.png"));
    this->setWindowIcon(QIcon(":/install/wallpaper.png"));



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
    QString commod = "ffmpeg -i " + path + " -ss 00:00:00.000 -vframes 1 -vf 'scale=256:144' " + thumPath +" |y";
    qDebug() << commod;
//    QProcess::execute(commod);
    if(!QFileInfo(thumPath).isFile()){
          system(commod.toStdString().c_str());
    }

    return true;
//    system("ffmpeg -i /opt/durapps/fantascene-dynamic-wallpaper/09.mp4 -ss 00:00:00.000 -vframes 1 -vf 'scale=256:144' /home/lmh/.config/fantascene-dynamic-wallpaper/.thumbnail/d18420fa260c7eff8fd0f2fac2f7b1cf.png");
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

    QPixmap  wallPix(QFileInfo(path).path()+"/"+"preview.jpg");
    if(!wallPix.isNull()){
        return wallPix.scaled(256,144);
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
    system("qdbus --literal com.deepin.dde.desktop /com/deepin/dde/desktop com.deepin.dde.desktop.EnableBackground false");
    //设置desktop透明
    char str[12] = "dde-desktop";
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
