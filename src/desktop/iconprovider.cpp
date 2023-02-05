/*
 * desktop-file-memos: A desktop classification app on Linux.
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
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
 *
 * Author: Yue Lan<lanyue@kylinos.cn>
 *
 */
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include "iconprovider.h"

#include <QMimeDatabase>
#include <QUrl>

#include <QImage>
#include <QImageReader>
#include <QDebug>

IconProvider::IconProvider()
{
    mimeDatabase = new QMimeDatabase;
}

IconProvider::~IconProvider()
{
    delete mimeDatabase;
}

void IconProvider::setIconSize(int size)
{
    mIconSize = size;
}

int IconProvider::getIconSize()
{
    return mIconSize;
}

QIcon IconProvider::icon(const QFileInfo &info) const
{

    QIcon fileIcon;

    //image file, return a thumbnail.
    QPixmap tmpPixmap;
    QImageReader reader(info.filePath());
    if (reader.size().width() <= 4096 && reader.size().height() <= 4096) {
        tmpPixmap = QPixmap::fromImage(reader.read());
    }
    if (!tmpPixmap.isNull()) {
        tmpPixmap = tmpPixmap.scaled(mIconSize, mIconSize);
        fileIcon = QIcon(tmpPixmap);
        return fileIcon;
    }

    //we need do something special to '*.desktop' file.
    if (mimeDatabase->mimeTypeForFile(info).iconName() == QString("application-x-desktop")) {
        std::string tmp_str = info.filePath().toStdString();
        const char *file_path = tmp_str.c_str();
        GDesktopAppInfo *desktop_app_info = g_desktop_app_info_new_from_filename(file_path);
        if (desktop_app_info != nullptr) {
            char *tmp_icon_name = g_desktop_app_info_get_string(desktop_app_info, "Icon");
            QIcon desktopFileIcon = QIcon::fromTheme(QString(tmp_icon_name));
            if (desktopFileIcon.isNull()) {
                QPixmap pixmap = QPixmap(QString(tmp_icon_name));
                if (pixmap.isNull()) {
                    fileIcon = QIcon::fromTheme("application-x-desktop");
                } else {
                    pixmap = pixmap.scaled(mIconSize, mIconSize);
                    QIcon pixmapIcon = QIcon(pixmap);
                    fileIcon = pixmapIcon;
                }
            } else {
                fileIcon = desktopFileIcon;
            }
            if (tmp_icon_name != nullptr) {
                g_free(tmp_icon_name);
            }
        }
        return fileIcon;
    }
    QString filePath = info.filePath();
    std::string str = filePath.toStdString();
    const char *file_path = str.c_str();
    GFile *g_file = g_file_new_for_path(file_path);
    GFileInfo *file_info = g_file_query_info(g_file,
                                             G_FILE_ATTRIBUTE_STANDARD_ICON,
                                             G_FILE_QUERY_INFO_NONE,
                                             nullptr,
                                             nullptr);

    GIcon *g_icon = g_file_info_get_icon(file_info);
    const gchar *const *icon_names = g_themed_icon_get_names(G_THEMED_ICON(g_icon));
    QIcon icon;
    if (icon_names != nullptr) {
        QString iconName = QString(*icon_names);
        while (iconName.contains(".") || iconName.contains("/")) {
            qDebug() << *icon_names;
            icon_names++; //we need use second string, first string is not correct some times (for example, a wps-office-doc).
            iconName = QString(*icon_names);
        }
        icon = QIcon::fromTheme(iconName);
    }

    g_object_unref(file_info);
    g_object_unref(g_file);

    return icon;
    //}

}
