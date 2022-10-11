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
#include <QDir>
#include <QUrl>
#include <QMutexLocker>
#include <QWindow>
#include <QDebug>
#include <QCryptographicHash>
#include <QTimer>
#include <QPainter>

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
    QSize size(256, 144); //指定图片大小;
    QImage image(size, QImage::Format_ARGB32); //以ARGB32格式构造一个QImage,
    //image.fill(qRgba(0,0,0,100));//填充图片背景,120/250为透明度
    QPainter painter(&image); //为这个QImage构造一个QPainter
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    //设置画刷的组合模式CompositionMode_SourceOut这个模式为目标图像在上。
    //改变组合模式和上面的填充方式可以画出透明的图片。
    //改变画笔和字体
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    QFont font = painter.font();
    font.setBold(true);//加粗
    font.setPixelSize(30);//改变字体大小
    painter.setPen(pen);
    painter.setFont(font);
    painter.drawText(image.rect(), Qt::AlignCenter, text);
    //将Hello写在Image的中心
//    int n = 100;//这个为图片的压缩度。0/100
//    image.save("text.png", "PNG", n);

    return QPixmap::fromImage(image);
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

    return getThumbnailText(path);
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
