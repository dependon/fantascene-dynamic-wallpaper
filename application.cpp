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
    this->setApplicationName(tr("deepin-dreamscene-ui"));
    this->setApplicationDisplayName(tr("deepin-dreamscene-ui"));
    this->setApplicationDescription(tr("deepin-dreamscene-ui\n"
                                       "基于qt5.11.3版本\n"
                                       "基于justforlxz的渲染方法开发\n"
                                       "github地址:https://github.com/dependon/deepin-dreamscene-ui/\n"
                                       "gitee地址:https://gitee.com/liuminghang/deepin-dreamscene-ui\n"
                                       "bug可以联系mail: liuminghang0821@gmail.com\n"
                                       "nvdia闭源驱动电脑下某些情况需要自定义模式配置尺寸"));
    this->setProductIcon(QIcon(":/tray.ico"));

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
    const QUrl url = QUrl::fromLocalFile(path);
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
