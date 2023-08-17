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

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QFileSystemModel>

class IconProvider;

class FileModel : public QFileSystemModel {
public:
    explicit FileModel(QObject *parent = nullptr);
    IconProvider *getIconProvider() {return mIconProvider;}

private:
    IconProvider *mIconProvider = nullptr;
};

#endif // FILEMODEL_H
