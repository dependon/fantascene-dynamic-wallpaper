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

public Q_SLOTS:
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
