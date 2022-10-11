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
#ifndef WALLPAPERENGINEPLUGIN_H
#define WALLPAPERENGINEPLUGIN_H

#include <QWidget>
#include <QMap>

struct wallpaperEengineData {
    QString JsonPath;
    QString JsonBasePath;
    QString descrption;
    QString path;
    QString general;
    QString preview;
    QString tags;
    QString title;
    QString type;
    QString visibility;
    QString file;
};
class view;
namespace Ui {
class wallpaperEnginePlugin;
}

class wallpaperEnginePlugin : public QWidget
{
    Q_OBJECT

public:
    explicit wallpaperEnginePlugin(QWidget *parent = nullptr);
    ~wallpaperEnginePlugin();

    void FindFile(const QString &_filePath);
    void readJson(const QString &path);
    void showView();

    void refresh(const QString &path);
private Q_SLOTS:
    void on_setEnginePath_clicked();

    void on_setBtn_clicked();

private:
    Ui::wallpaperEnginePlugin *ui;

    QStringList m_JasonList;
    QMap <QString, wallpaperEengineData> m_map;

    view *m_view{nullptr};

};

#endif // WALLPAPERENGINEPLUGIN_H
