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
#ifndef DATA_H
#define DATA_H
#include <QString>
enum ScreenMode {
    IdCopyScreen,
    IdlayoutScreen,
    IdManualSet
};

struct MoreSetData {
    int isAuto = 0;
    int fps = 10;
    QString vo = "";
    QString hwdec = "gpu";
    bool isShowDesktopIcon = true;
    bool isTop = true;
    bool isEventPenetration = false;
    double m_DesktopTransparency = 0.99;
    double m_WallpaperTransparency = 0.99;
    QString fontColor ="default";
    QString language  ="";
};

struct VideoData {
    QString md5;
    QString name;
    QString description;
    QString category;
    QString author;
    QString fileName;
    QString downloadPath;
    int downloadCount;
    qint64 filesize;
    int width;
    int height;
    QByteArray picture; // Assuming picture is stored as a binary object
};

struct DownloadInfo
{
    QString url; //原始下载地址
    QString outputFilePath; //保存名称
    QString extraPath; //额外信息
    QString dowloadCode; //下载指令
    int downloadIngTaskCount = 0; //正在下载的任务数量
    bool bDownloaded = false; //下载成功与否
};

#endif // DATA_H
