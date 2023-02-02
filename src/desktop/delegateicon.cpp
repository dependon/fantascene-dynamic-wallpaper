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

#include "delegateicon.h"
#include "fileoperationjob.h"

#include <QTextEdit>
#include <QStaticText>
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

#include <QThread>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QFontMetrics>

DelegateIcon::DelegateIcon()
{

}

QString DelegateIcon::displayText(const QVariant &value, const QLocale &locale) const
{
    QString text = value.toString();
    if (!text.endsWith(QString(".desktop"))) {
        return text;
    }
    QString keyWord = "Name[" + locale.name() + "]";
    std::string tmp_key = keyWord.toStdString();

    QString filePath = mModel->filePath(mIconView->rootIndex()) + "/" + text;
    std::string tmp_str = filePath.toStdString();
    const char *file_path = tmp_str.c_str();
    GDesktopAppInfo *desktop_app_info = g_desktop_app_info_new_from_filename(file_path);

    if (desktop_app_info != nullptr) {
        char *tmp_name = g_desktop_app_info_get_string(desktop_app_info, tmp_key.c_str());
        if (tmp_name != nullptr) {
            text = QString(tmp_name);
            g_free(tmp_name);
        } else {
            tmp_name = g_desktop_app_info_get_string(desktop_app_info, "Name");
            if (tmp_name != nullptr) {
                text = QString(tmp_name);
                g_free(tmp_name);
            }
        }

        g_object_unref(desktop_app_info);
    }
    return text;
}

QWidget *DelegateIcon::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QTextEdit *textEdit = new QTextEdit(parent);
    textEdit->setStyleSheet("background-color:white;"
                            "color:black;");
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setAlignment(Qt::AlignHCenter);
    textEdit->setText(index.data().toString());
    textEdit->setFixedWidth(option.rect.width());
    textEdit->setFixedHeight(150);

    connect(textEdit, &QTextEdit::destroyed, [ = ]() {

        if (!textEdit->toPlainText().isEmpty()) {
            QString srcPath = mModel->filePath(mIconView->rootIndex()) + "/" + index.data().toString();
            QString destPath = mModel->filePath(mIconView->rootIndex()) + "/" + textEdit->toPlainText();
            QFileInfo info(srcPath);
            if (info.isDir()) {
                QDir dir;
                dir.rename(srcPath, destPath);
            } else {
                QFile file;
                file.rename(srcPath, destPath);
            }
        }
    });
    return textEdit;
}

void DelegateIcon::setEditorData(QWidget *, const QModelIndex &) const
{
    return;
}

void DelegateIcon::setModelData(QWidget *, QAbstractItemModel *, const QModelIndex &) const
{
    return;
}

void DelegateIcon::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

QSize DelegateIcon::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option,index);
    Q_UNUSED(index)
    return QSize(option.rect.width(), option.rect.height());
}

void DelegateIcon::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    return QStyledItemDelegate::paint(painter,option,index);
    if(!index.isValid())
    {
        return QStyledItemDelegate::paint(painter,option,index);
    }

    painter->save();

    qDebug()<<option.state;
    //选中
    if(QStyle::State_Selected & option.state)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    //图标
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    QRect iconRect = option.rect;


//    qDebug()<<option.rect.width();
//    qDebug()<<iconRect.width();
//    qDebug()<<iconRect;
    iconRect.setX(option.rect.width()/4+option.rect.x());
    iconRect.setSize(QSize(option.rect.width()/2, option.rect.width()/2));
//    qDebug()<<iconRect;
//    iconRect.setX((iconRect.height()-iconRect.x())/2);
    QIcon::State state;
    if(option.state == QStyle::State_Open)
    {
        state = QIcon::On;
    }
    else
    {
        state = QIcon::Off;
    }
    icon.paint(painter, iconRect, Qt::AlignCenter | Qt::AlignVCenter, QIcon::Selected,state);

    //text
    QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics fontMetrics(qApp->font());
    int padding = 8;
//    QRect rect = fontMetrics.boundingRect(option.rect.left()+padding/2, option.rect.bottom()-iconRect.height()+padding/2,
//                                             option.rect.width()-padding, option.rect.height()-padding,
//                                             Qt::AlignHCenter | Qt::AlignBottom | Qt::TextWrapAnywhere,
//                                             text);
    QRect rect = fontMetrics.boundingRect(option.rect.left()+padding/2, option.rect.top()+iconRect.height()+padding/2,
                                             option.rect.width()-padding, option.rect.height()-padding,
                                             Qt::AlignHCenter | Qt::AlignTop | Qt::TextWrapAnywhere,
                                             text);

//    qDebug()<<rect;
    QColor color = qApp->palette().text().color();
    color=Qt::white;
    QPen pen(color);
    QString caiText = fontMetrics.elidedText(text,Qt::ElideRight,option.rect.width()*2-20);
    painter->setPen(pen);
    painter->setFont(qApp->font());
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWrapAnywhere, caiText);
    painter->restore();
}
