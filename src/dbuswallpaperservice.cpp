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
#include "dbuswallpaperservice.h"
#include "wallpaper.h"

#include "application.h"
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

void DBusWallpaperService::activeWindow()
{
    Q_EMIT dApp->sigActiveWindow();
}
