#include "application.h"
#include <QIcon>
#include <QDir>
#include <QUrl>
#include <QMutex>
#include <QMutexLocker>
#include <QCryptographicHash>

const QString toMd5(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
}

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    this->setApplicationVersion("Version 0.1.4");
    this->setApplicationName(tr("deepin-dreamscene-ui"));
    this->setApplicationDisplayName(tr("deepin-dreamscene-ui"));
    this->setApplicationDescription(
                    QObject::tr(
                        "<span style='font-size:10pt;font-weight:60;'>wallpaper by deepin community</span><br/>"
                        "<a href='https://github.com/dependon/deepin-dreamscene-ui/'>github/deepin-dreamscene-ui</a><br/>"
                        "<a href='https://gitee.com/liuminghang/deepin-dreamscene-ui/'>gitee/deepin-dreamscene-ui</a><br/>"
                        "<span style='font-size:12pt;'>qt5.15,deepinv20.2</span><br/><br/>"
                        "<span style='font-size:12pt;'>mail: liuminghang0821@gmail.com</span><br/><br/>"
                        "Published under GPL V3"
                        )
                    );

    this->setProductIcon(QIcon(":/tray.ico"));
    this->setWindowIcon(QIcon(":/tray.ico"));

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

QMutex mutex;
const QPixmap Application::getThumbnail(const QString &path)
{
    QMutexLocker locker(&mutex);

    const QString cacheP = thumbnailCachePath();
    QUrl url;
    if (!path.contains("file://")) {
        url = QUrl::fromLocalFile(path);
    } else {
        url = QUrl(path);
    }
    const QString md5s = toMd5(url.toString(QUrl::FullyEncoded).toLocal8Bit());
    const QString encodePath = cacheP + "/large/" + md5s + ".png";
//    const QString failEncodePath = cacheP + "/fail/" + md5s + ".png";
    if (QFileInfo(encodePath).exists()) {
        return QPixmap(encodePath);
    } else { /*if (QFileInfo(failEncodePath).exists()) */
        qDebug() << "Fail-thumbnail exist, won't regenerate: " ;
        return QPixmap();
    }
}
