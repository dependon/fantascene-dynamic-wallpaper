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

#ifndef FILEOPERATIONJOB_H
#define FILEOPERATIONJOB_H

#define FILE_OPERATION_MOVE 0
#define FILE_OPERATION_COPY 1
#define FILE_OPERATION_TRASH 2
#define FILE_OPERATION_DELETE 3
#define FILE_OPERATION_SENDTO_DESKTOP 4
#define FILE_OPERATION_SENDTO_DESKTOP_AND_TRASH 5

#include <QList>
#include <QUrl>
#include <QObject>

class FileOperationJob : public QObject
{
    Q_OBJECT
public:
    explicit FileOperationJob(QObject *parent = nullptr);
    void setOrigList (QList<QUrl> urls) {origList = urls;}
    void setOrigList (QStringList pathList) {origList1 = pathList;}
    void setDestDir (QString path) {destDir = path;}
    void setOperationFlag (int flag) {opFlag = flag;}

Q_SIGNALS:
    void jobDone();

public Q_SLOTS:
    void jobStart();

private:
    QList<QUrl> origList;
    QStringList origList1;
    QString destDir = nullptr;
    int opFlag = -1;
};

#endif // FILEOPERATIONJOB_H
