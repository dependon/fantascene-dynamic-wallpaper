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
#include <QPainterPath>
#include <QClipboard>
#include <QMimeData>

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
    if(!index.isValid())
    {
        return QStyledItemDelegate::paint(painter,option,index);
    }
    painter->setRenderHints(QPainter::HighQualityAntialiasing |
                            QPainter::SmoothPixmapTransform |
                            QPainter::Antialiasing);

    painter->save();

    qDebug()<<option.state;
    //选中
    if(QStyle::State_Selected & option.state)
    {
        QRect backgroundRect = option.rect;
        QPainterPath backgroundBp;
        backgroundBp.addRoundedRect(backgroundRect, 12, 12);
        painter->setClipPath(backgroundBp);
        painter->fillRect(backgroundRect, option.palette.highlight());
    }

    //是否有剪切

    bool isCut =false ;
    QByteArray ba = QApplication::clipboard()->mimeData()->data("x-special/gnome-copied-files");
    QString tStr(ba);
    if (tStr.startsWith("cut")) {
        QList<QUrl> urls = QApplication::clipboard()->mimeData()->urls();
        QString text = index.data(Qt::DisplayRole).toString();
        QString filePath = mModel->filePath(mIconView->rootIndex()) + "/" + text;
        for (QUrl url :urls)
        {
            QString localPath = url.toLocalFile();
            if(localPath == filePath)
            {
                isCut = true;
            }
        }
    }

    //图标
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    QRect iconRect = option.rect;
    iconRect.setX(option.rect.width()/4+option.rect.x());
    iconRect.setSize(QSize(option.rect.width()/2, option.rect.width()/2));
    QIcon::State state;
    if(option.state == QStyle::State_Open)
    {
        state = QIcon::On;
    }
    else
    {
        state = QIcon::Off;
    }
    {
        icon.paint(painter, iconRect, Qt::AlignCenter | Qt::AlignVCenter, QIcon::Active,state);
    }
    //是剪切文件且未被选中
    if(isCut && !(QStyle::State_Selected & option.state))
    {
        painter->fillRect(iconRect,QColor(0,0,0,125));
    }


    //text
    QString text = displayText(index.data(Qt::DisplayRole),QLocale::system());
    QFontMetrics fontMetrics(qApp->font());
    int padding = 8;
    QRect rect = fontMetrics.boundingRect(option.rect.left()+padding/2, option.rect.top()+iconRect.height(),
                                             option.rect.width()-padding, option.rect.height()-padding,
                                             Qt::AlignHCenter | Qt::AlignTop | Qt::TextWrapAnywhere,
                                             text);

    QColor color = qApp->palette().text().color();
    color=Qt::white;
    QPen pen(color);
    QString caiText = fontMetrics.elidedText(text,Qt::ElideRight,option.rect.width()*2-20);
    painter->setPen(pen);
    painter->setFont(qApp->font());
    painter->drawText(rect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextWrapAnywhere, caiText);
    painter->restore();
}
