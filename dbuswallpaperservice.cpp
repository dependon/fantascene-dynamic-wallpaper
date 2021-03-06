#include "dbuswallpaperservice.h"
#include "wallpaper.h"

DBusWallpaperService::DBusWallpaperService(Wallpaper *parent)
    : QDBusAbstractAdaptor(parent),
      m_parent(parent)
{

}

Wallpaper *DBusWallpaperService::parent() const
{
    return m_parent;
}

void DBusWallpaperService::setFile(const QString &path)
{
    m_parent->setFile(path);
}

void DBusWallpaperService::setVolume(const qint32 volume)
{
    m_parent->setVolume(volume);
}

void DBusWallpaperService::clear()
{
    m_parent->clear();
}

void DBusWallpaperService::play()
{
    m_parent->play();
}

void DBusWallpaperService::pause()
{
    m_parent->pause();
}

void DBusWallpaperService::stop()
{
    m_parent->stop();
}
