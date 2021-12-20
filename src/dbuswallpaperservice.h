#ifndef DBUSWALLPAPERSERVICE_H
#define DBUSWALLPAPERSERVICE_H

#include <QObject>
#include <QtDBus/QtDBus>

class Wallpaper;
class DBusWallpaperService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.dde.fantascene")
public:
    explicit DBusWallpaperService(Wallpaper *parent);

    inline Wallpaper *parent() const;

public slots:
    void setFile(const QString &path);
    void setVolume(const qint32 volume);
    void clear();
    void play();
    void pause();
    void stop();

    void activeWindow();

private:
    Wallpaper *m_parent;
};

#endif // DBUSWALLPAPERSERVICE_H
