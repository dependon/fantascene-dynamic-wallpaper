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

#include "fileoperationjob.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <gio/gio.h>

#include <QDebug>

static bool copyRecursively(QString srcFilePath, QString tgtFilePath);

FileOperationJob::FileOperationJob(QObject *parent) : QObject(parent)
{

}

void FileOperationJob::jobStart()
{
    if (origList.isEmpty() && origList1.isEmpty()) {
        return;
    }
    switch (opFlag) {
    case FILE_OPERATION_MOVE:
        //move
        if (destDir == nullptr) {
            break;
        }
        if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QUrl srcFileUrl = origList.at(i);

                QFileInfo tmpInfo = QFileInfo(srcFileUrl.path());
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                if (tmpInfo.isFile()) {
                    qDebug() << "is file";
                    qDebug() << QFile::rename(sourcePath, destPath);
                } else if (tmpInfo.isDir()) {
                    qDebug() << "is dir";
                    qDebug() << QDir(sourcePath).rename(sourcePath, destPath);
                } else {
                    qDebug() << "tmpInfo has not type?";
                }
            }
        } else if (!origList1.isEmpty()) {

        }
        break;

    case FILE_OPERATION_COPY:
        //copy
        if (destDir == nullptr) {
            break;
        }
        if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QUrl srcFileUrl = origList.at(i);

                QFileInfo tmpInfo = QFileInfo(srcFileUrl.path());
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                copyRecursively(sourcePath, destPath);
            }
        } else if (!origList1.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QString srcPath = origList.at(i).path();
                QFileInfo tmpInfo = QFileInfo(srcPath);
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + filename;

                copyRecursively(sourcePath, destPath);
            }
        }
        break;

    case FILE_OPERATION_TRASH:
        //trash
        if (!origList1.isEmpty()) {
            for (int i = 0; i < origList1.count(); i++) {
                QString srcPath = origList1.at(i);

                std::string tmp_str = srcPath.toStdString();
                GFile *file = g_file_new_for_path(tmp_str.c_str());
                g_file_trash(file, nullptr, nullptr);
                g_object_unref(file);
            }
        } else if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QUrl srcUrl = origList.at(i);
                qDebug() << srcUrl;

                QString tmpStr = srcUrl.toString();
                std::string tmp_str = tmpStr.toStdString();
                GFile *file = g_file_new_for_uri(tmp_str.c_str());
                g_file_trash(file, nullptr, nullptr);
                g_object_unref(file);
            }
        }
        break;

    case FILE_OPERATION_DELETE:
        //delete
        if (!origList1.isEmpty()) {
            for (int i = 0; i < origList1.count(); i++) {
                QString srcPath = origList1.at(i);

                QFileInfo info(srcPath);
                if (info.isDir()) {
                    QDir dir(srcPath);
                    dir.removeRecursively();
                } else {
                    QFile(srcPath).remove();
                }
            }
        } else if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QUrl srcUrl = origList.at(i);
                qDebug() << srcUrl;

                QString srcPath = srcUrl.path();
                QFileInfo info(srcPath);
                if (info.isDir()) {
                    QDir dir(srcPath);
                    dir.removeRecursively();
                } else {
                    QFile(srcPath).remove();
                }
            }
        }
        break;

    case FILE_OPERATION_SENDTO_DESKTOP:
        destDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        if (!origList1.isEmpty()) {
            for (int i = 0; i < origList1.count(); i++) {
                QString srcPath = origList1.at(i);

                QFileInfo tmpInfo = QFileInfo(srcPath);
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                if (tmpInfo.isFile()) {
                    qDebug() << "is file";
                    qDebug() << QFile::rename(sourcePath, destPath);
                } else if (tmpInfo.isDir()) {
                    qDebug() << "is dir";
                    qDebug() << QDir(sourcePath).rename(sourcePath, destPath);
                } else {
                    qDebug() << "tmpInfo has not type?";
                }
            }
        } else if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QString srcPath = origList.at(i).path();

                QFileInfo tmpInfo = QFileInfo(srcPath);
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                if (tmpInfo.isFile()) {
                    qDebug() << "is file";
                    qDebug() << QFile::rename(sourcePath, destPath);
                } else if (tmpInfo.isDir()) {
                    qDebug() << "is dir";
                    qDebug() << QDir(sourcePath).rename(sourcePath, destPath);
                } else {
                    qDebug() << "tmpInfo has not type?";
                }
            }
        }
        break;

    case FILE_OPERATION_SENDTO_DESKTOP_AND_TRASH:
        destDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        if (!origList1.isEmpty()) {
            for (int i = 0; i < origList1.count(); i++) {
                QString srcPath = origList1.at(i);

                QFileInfo tmpInfo = QFileInfo(srcPath);
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                if (tmpInfo.isFile()) {
                    qDebug() << "is file";
                    qDebug() << QFile::rename(sourcePath, destPath);
                } else if (tmpInfo.isDir()) {
                    qDebug() << "is dir";
                    qDebug() << QDir(sourcePath).rename(sourcePath, destPath);
                } else {
                    qDebug() << "tmpInfo has not type?";
                }

                std::string tmp_str = destPath.toStdString();
                GFile *file = g_file_new_for_path(tmp_str.c_str());
                g_file_trash(file, nullptr, nullptr);
                g_object_unref(file);
            }
        } else if (!origList.isEmpty()) {
            for (int i = 0; i < origList.count(); i++) {
                QString srcPath = origList.at(i).path();

                QFileInfo tmpInfo = QFileInfo(srcPath);
                QString sourcePath = tmpInfo.filePath();

                QString filename = tmpInfo.fileName();
                QString destPath = destDir + "/" + filename;

                if (tmpInfo.isFile()) {
                    qDebug() << "is file";
                    qDebug() << QFile::rename(sourcePath, destPath);
                } else if (tmpInfo.isDir()) {
                    qDebug() << "is dir";
                    qDebug() << QDir(sourcePath).rename(sourcePath, destPath);
                } else {
                    qDebug() << "tmpInfo has not type?";
                }

                std::string tmp_str = destPath.toStdString();
                GFile *file = g_file_new_for_path(tmp_str.c_str());
                g_file_trash(file, nullptr, nullptr);
                g_object_unref(file);
            }
        }
        break;

    default:
        break;
    }
    Q_EMIT jobDone();
}

bool copyRecursively(QString srcFilePath, QString tgtFilePath)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir()) {
        QDir targetDir(tgtFilePath);
        bool ok = targetDir.cdUp();
        QDir dir;
        while (dir.exists(tgtFilePath)) {
            tgtFilePath.append("(copy)");
        }
        if (ok && !targetDir.mkdir(QFileInfo(tgtFilePath).fileName())) return false;

        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        for (int i = 0; i < fileNames.count(); i++) {
            const QString newSrcFilePath = srcFilePath + QLatin1Char('/') + fileNames.at(i);
            const QString newTgtFilePath = tgtFilePath + QLatin1Char('/') + fileNames.at(i);
            if (!copyRecursively(newSrcFilePath, newTgtFilePath)) return false;
        }
    } else {
        while (QFile::exists(tgtFilePath)) {
            QFileInfo info(tgtFilePath);
            QString name = info.completeBaseName();
            name.append("(copy)");
            tgtFilePath = info.dir().path() + "/" + name + "." + info.completeSuffix();
        };
        if (!QFile::copy(srcFilePath, tgtFilePath)) {
            return false;
        }
    }

    return true;
}
