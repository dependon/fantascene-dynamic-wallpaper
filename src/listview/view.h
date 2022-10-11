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
#ifndef VIEW_H
#define VIEW_H

#include <QStandardItemModel>
#include <QBuffer>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QListView>
#include "delegate.h"

class view: public QListView
{
    Q_OBJECT
public:
    struct ItemInfo {
        QString name = "";
        QString path = "";
        int width = 0;
        int height = 0;
        QPixmap image = QPixmap();

        friend bool operator== (const ItemInfo &left, const ItemInfo &right)
        {
            if (left.image == right.image)
                return true;
            return false;
        }
    };
    explicit view(QWidget *parent = nullptr);
    void setFiles(const QStringList &pathlist);
    ~view() override;

    void modifyAllPic(ItemInfo &info);
    void cutPixmap(ItemInfo &iteminfo);
    void calgridItemsWidth();

    void refresh();

    QStandardItemModel *m_model = nullptr;
    delegate *m_delegate = nullptr;
    QList<ItemInfo> m_allItemInfo;
    int m_iBaseHeight = 100;
    int rowSizeHint = 0;
    QList<ItemInfo> m_allItemLeft;//所有待处理的

public Q_SLOTS:
    void setBaseHeight(int a);
    void removePath(const QString &path);
    void addPath(const QString &path);
private Q_SLOTS:
    void onDoubleClicked(const QModelIndex &index);
    void onClicked(const QModelIndex &index);
protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) override;

private:
    int m_currentModelIndex{0};
};

#endif // VIEW_H
