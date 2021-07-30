#include "delegate.h"

#include <QHBoxLayout>

#include <QPainter>
#include <QPixmapCache>
#include <QStandardItemModel>
#include <QThread>
#include <QTimer>
#include <QPainterPath>
#include <QMouseEvent>
#include <QImageReader>
#include <QApplication>
#include <DGuiApplicationHelper>
#include <DWidget>

DWIDGET_USE_NAMESPACE

delegate::delegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , selectedPixmap(":/select_active.svg")
{

}

void delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    const ItemData data = itemData(index);
    bool selected = data.isSelected;
    if (/*(option.state & QStyle::State_MouseOver) &&*/
        (option.state & QStyle::State_Selected) != 0) {
        selected = true;
    }
    painter->setRenderHints(QPainter::HighQualityAntialiasing |
                            QPainter::SmoothPixmapTransform |
                            QPainter::Antialiasing);
    QRect backgroundRect = option.rect;

    //选中阴影框
    if (selected) {
        QPainterPath backgroundBp;
        backgroundBp.addRoundedRect(backgroundRect, 8, 8);
        painter->setClipPath(backgroundBp);

        QBrush  shadowbrush;
        QPixmap selectedPixmap;

        shadowbrush = QBrush(QColor("#DEDEDE"));

        painter->fillRect(backgroundRect, shadowbrush);

        //绘制选中默认背景
        QRect backRect(backgroundRect.x() + 8, backgroundRect.y() + 8, backgroundRect.width() - 16, backgroundRect.height() - 16);
        QPainterPath backBp;
        backBp.addRoundedRect(backRect, 8, 8);
        painter->setClipPath(backBp);
        painter->fillRect(backRect, shadowbrush);
    }


    QBrush transparentbrush;
    transparentbrush = QBrush(QColor("#FFFFFF"));

    QRect transparentRect(backgroundRect.x() + 8, backgroundRect.y() + 8, backgroundRect.width() - 16, backgroundRect.height() - 16);
    QPainterPath transparentBp;
    transparentBp.addRoundedRect(transparentRect, 8, 8);
    painter->setClipPath(transparentBp);
    painter->fillRect(transparentRect, transparentbrush);


    QRect pixmapRect;

    pixmapRect.setX(backgroundRect.x() + 8);
    pixmapRect.setWidth(backgroundRect.width() - 16);
    pixmapRect.setY(backgroundRect.y() + 8);;
    pixmapRect.setHeight(backgroundRect.height() - 16);

    QPainterPath bp1;
    bp1.addRoundedRect(pixmapRect, 8, 8);
    painter->setClipPath(bp1);
    painter->drawPixmap(pixmapRect, data.image);


    if (selected) {
        QRect selectedRect(backgroundRect.x() + backgroundRect.width() - 30, backgroundRect.y() + 4, 28, 28);
        QPainterPath selectedBp;
        selectedBp.addRoundedRect(selectedRect, 8, 8);
        painter->setClipPath(selectedBp);
        painter->drawPixmap(selectedRect, selectedPixmap);
    }

    painter->restore();



}

QSize delegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(100, 100);
}

//bool delegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
//{
//    Q_UNUSED(model);
//    if (!index.isValid())
//        return false;
//    QRect rect = QRect(option.rect.x() + option.rect.width() - 20 - 13 - 2, option.rect.y() + option.rect.height() - 20 - 10 - 2, 20, 20);
//    QMouseEvent *pMouseEvent = static_cast<QMouseEvent *>(event);
//    if (event->type() == QEvent::MouseButtonPress) {
//        const ItemData data = itemData(index);
//        bool blast = false;
//        if (!blast && rect.contains(pMouseEvent->pos())) {
//            //                emit sigCancelFavorite(index);
//        } else if (blast && event->type() == QEvent::MouseButtonPress && rect.contains(pMouseEvent->x(), pMouseEvent->y() + 27)) {
//            //                emit sigCancelFavorite(index);
//        }
//    }
//    return false;
//}

delegate::ItemData delegate::itemData(const QModelIndex &index) const
{
    QVariantList datas = index.model()->data(index, Qt::DisplayRole).toList();
    ItemData data;
    if (datas.length() >= 1) {
        data.name = datas[0].toString();
    }
    if (datas.length() >= 2) {
        data.path =  datas[1].toString();
    }
    if (datas.length() >= 3) {
        data.width = datas[2].toInt();
    }
    if (datas.length() >= 4) {
        data.height = datas[3].toInt();
    }
    if (datas.length() >= 5) {
        data.image = datas[4].value<QPixmap>();
    }
    return data;
}
