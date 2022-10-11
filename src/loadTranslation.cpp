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
/*
 * Check instance lock path
 *
 * @ Accepts parameters are : path (const), lock (const)
 *
 * @ Returns true if status checks are successful
 */

#include "loadTranslation.h"

#include <QDir>
#include <QDirIterator>
#include <QLockFile>
#include <QDebug>
#include <QApplication>
#include <QTranslator>

#ifdef Q_OS_LINUX
/*
 * Load translation files
 *
 * @ Accepts parameters are : path (const)
 *
 * @ Returns no results
 */
void load_translation_files(const QString path)
{
    QDir dir(path);
    if (dir.exists())
    {
        QDirIterator qmIt(path, QStringList() << QString("*%1.qm").arg(QLocale::system().name()), QDir::Files);
        while (qmIt.hasNext())
        {
            qmIt.next();
            QFileInfo finfo = qmIt.fileInfo();
            QString tr_path = finfo.absolutePath();
            QString tr_file = finfo.baseName();
            QTranslator *translator = new QTranslator;

            if (!translator->load(tr_file,tr_path)) {
                qWarning() << "Failed to load " + tr_path, + "/" + tr_file + " ,fallback to default!";
            }
            if (!qApp->installTranslator(translator)) {
                qWarning() << "Failed to install translation!";
            }
        }
    }
}
#endif
