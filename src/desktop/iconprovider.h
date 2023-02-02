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

#ifndef ICONPROVIDER_H
#define ICONPROVIDER_H

#include <QFileIconProvider>

class QMimeDatabase;

class IconProvider : public QFileIconProvider
{
public:
    IconProvider();
    ~IconProvider();
    void setIconSize(int size);
    int getIconSize();
    QIcon icon(const QFileInfo &info) const;

private:
    int mIconSize = 90;
    QMimeDatabase *mimeDatabase = nullptr;
};

#endif // ICONPROVIDER_H
