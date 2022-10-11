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
#ifndef DELEGATE_H
#define DELEGATE_H
#include <QObject>
#include <QDateTime>
#include <QStyledItemDelegate>

class delegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    struct ItemData {
        QString name;
        QString path = QString();
        int width;
        int height;
        QPixmap image;
        bool isSelected{false};
        int baseHeight = 0;
        int baseWidth = 0;
        int imgWidth;
        int imgHeight;
    };

    explicit delegate(QObject *parent = nullptr);

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
//    bool editorEvent(QEvent *event,
//                     QAbstractItemModel *model,
//                     const QStyleOptionViewItem &option,
//                     const QModelIndex &index) Q_DECL_OVERRIDE;

    ItemData itemData(const QModelIndex &index) const;

    QPixmap selectedPixmap;
};

#endif // DELEGATE_H
