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

#ifndef ICONVIEW_H
#define ICONVIEW_H

#include "filemodel.h"
#include <QListView>

class QMenu;
class CustomSortFilterProxyModel;
enum CopyOrCut
{
    CopyOrCut_COPY,
    CopyOrCut_CUT
};

class IconView : public QListView
{
    Q_OBJECT

public:
    QString readSettings(const QString &path, const QString &group, const QString &key);
    IconView(int id, QString rootPath, QWidget *parent = nullptr);
    ~IconView() override;
    void copyImageToClipboard(const QStringList &paths , CopyOrCut type = CopyOrCut_COPY);
    QString terminalPath();

    void setIconTextSize(int size);
    //默认判断exe
    void openExeFile(bool isExe = true);

    void saveLayoutOrder();

    void restoreLayoutOrder();
private:
    int mId = -1;
    FileModel *fileModel = nullptr;
    CustomSortFilterProxyModel * m_proxyModel = nullptr;

    QPoint mLastPos = QPoint(0, 0);
    QString m_rootPath;

Q_SIGNALS :
    void sigMouseMove();
    void sigMouseClick(const int &index);
    void sigMouseDclick();

public Q_SLOTS:
    void copyFile();
    void cutFile();
    void pauseFile();
    void openFile();
    void deleteFile();
    void renameFile();
    void slotsnewFolder();
    void slotsnewTxt();
    void slotsopenTerminal();
    void slotsIconSizeSmall();
    void slotsIconSizeMedium();
    void slotsIconSizeBig();
    void slotsWallpaperAction();
    void slotsSort();

    void onActionTriggered();
    void onSetOhterActionTriggered();
    void onNewActionTriggered();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;
    void closeEvent(QCloseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void keyPressEvent(QKeyEvent *event) override;


private :
    QMenu *m_openSelect{nullptr};
    QMenu *m_createNew{nullptr};
};

#endif // ICONVIEW_H
