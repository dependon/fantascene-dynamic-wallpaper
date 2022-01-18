#include "view.h"
#include <QDebug>
#include <QDrag>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeData>
#include <QScrollBar>
#include <QMutex>
#include <QScroller>
#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <QScrollBar>
#include <QMessageBox>
#include "application.h"

const int ITEM_SPACING = 4;
const int BASE_HEIGHT = 100;
view::view(QWidget *parent)
{
    m_model = new QStandardItemModel(this);

    m_delegate = new delegate();
    setItemDelegate(m_delegate);
    setModel(m_model);

    m_iBaseHeight = BASE_HEIGHT;
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
    setSpacing(ITEM_SPACING);
    setDragEnabled(false);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(QListView::ScrollPerPixel);
    verticalScrollBar()->setSingleStep(20);

    setContextMenuPolicy(Qt::CustomContextMenu);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setViewportMargins(0, 0, -25, 0);
    connect(this, &view::clicked, this, &view::onClicked);
    connect(this, &view::doubleClicked, this, &view::onDoubleClicked);


}

void view::setFiles(const QStringList &pathlist)
{
    m_allItemInfo.clear();
    //文件
    for (QString str : pathlist) {
        ItemInfo item;
        item.name = QFileInfo(str).completeBaseName();
        item.path = str;
        item.image = dApp->getThumbnail(str);
        modifyAllPic(item);
        cutPixmap(item);
        m_allItemInfo << item;
    }
}

view::~view()
{
}

void view::modifyAllPic(view::ItemInfo &info)
{
    int i_totalwidth = width();
    info.width = m_iBaseHeight;
    info.height = m_iBaseHeight;
    if (info.width > i_totalwidth) {
        info.height = i_totalwidth / 4;
        info.width = i_totalwidth / 4;
    }
}

void view::cutPixmap(view::ItemInfo &iteminfo)
{
    int width = iteminfo.image.width();
    if (width == 0)
        width = m_iBaseHeight;
    int height = iteminfo.image.height();
    if (abs((width - height) * 10 / width) >= 1) {
        QRect rect = iteminfo.image.rect();
        int x = rect.x() + width / 2;
        int y = rect.y() + height / 2;
        if (width > height) {
            x = x - height / 2;
            y = 0;
            iteminfo.image = iteminfo.image.copy(x, y, height, height);
        } else {
            y = y - width / 2;
            x = 0;
            iteminfo.image = iteminfo.image.copy(x, y, width, width);
        }
    }
}

void view::calgridItemsWidth()
{
    qDebug() << "----calgridItemsWidth----调整图片大小1 " << width() << m_iBaseHeight << m_allItemInfo.length();
    int i_totalwidth = width();
    QList<ItemInfo> oneRowList;
    //计算一行的个数
    rowSizeHint = (i_totalwidth - ITEM_SPACING) / (m_iBaseHeight + ITEM_SPACING);
    int currentwidth = (i_totalwidth - ITEM_SPACING * (rowSizeHint + 1)) / rowSizeHint;//一张图的宽度
    int Remaining_pixels = (i_totalwidth - ITEM_SPACING * (rowSizeHint + 1)) % rowSizeHint;//剩余像素
    if (currentwidth < 80)
        currentwidth = 80;
    qDebug() << "----calgridItemsWidth----调整图片大小2 " << rowSizeHint << currentwidth << Remaining_pixels;

    for (int i = 0; i < m_allItemInfo.length(); i++) {
        if (Remaining_pixels > 0) {
            if (i % rowSizeHint < Remaining_pixels) {
                m_allItemInfo[i].width = currentwidth + 1;
                m_allItemInfo[i].height = currentwidth + 1;
            } else {
                m_allItemInfo[i].width = currentwidth;
                m_allItemInfo[i].height = currentwidth;
            }
        } else {
            m_allItemInfo[i].width = currentwidth;
            m_allItemInfo[i].height = currentwidth;
        }
    }
}

void view::refresh()
{
    m_model->clear();
    for (int i = 0; i < m_allItemInfo.length(); i++) {
        QVariantList datas;
        QStandardItem *item = new QStandardItem;
        datas.append(QVariant(m_allItemInfo[i].name));
        datas.append(QVariant(m_allItemInfo[i].path));
        datas.append(QVariant(m_allItemInfo[i].width));
        datas.append(QVariant(m_allItemInfo[i].height));
        datas.append(QVariant(m_allItemInfo[i].image));

        item->setData(QVariant(datas), Qt::DisplayRole);
        QStringList albumNames;
        albumNames << "all" << "vaa";
        item->setData(QVariant(albumNames), Qt::UserRole + 2);

        item->setData(QVariant(QSize(80, 80)),
                      Qt::UserRole + 5);
        m_model->appendRow(item);
    }
    this->setSpacing(ITEM_SPACING);     //重新布局
}

void view::removePath(const QString &path)
{
    for (ItemInfo info : m_allItemInfo) {
        if (info.path.contains(path)) {
            m_allItemInfo.removeOne(info);
        }
    }
}

void view::addPath(const QString &path)
{
    bool bRet = true;
    for (ItemInfo info : m_allItemInfo) {
        if (info.path.contains(path)) {
            bRet = false;
        }
    }
    if (bRet) {
        ItemInfo item;
        item.name = QFileInfo(path).completeBaseName();
        item.path = path;
        item.image = dApp->getThumbnail(path);
        modifyAllPic(item);
        cutPixmap(item);
        m_allItemInfo << item;
        refresh();
    }

}

void view::resizeEvent(QResizeEvent *e)
{
    refresh();
}

void view::setBaseHeight(int a)
{
    m_iBaseHeight = a;
}

void view::onDoubleClicked(const QModelIndex &index)
{
    int i = index.row();
    if (m_allItemInfo.size() > 0) {
        ItemInfo info = m_allItemInfo.at(i);
        QString str = info.path;
        qDebug() << str;
        QFileInfo fileInfo(str);
        if (fileInfo.isFile()) {
            dApp->setWallPaper(str);
            dApp->saveSetting();
        } else {
            if (QMessageBox::Yes == QMessageBox::information(NULL, tr("Delete!!"), tr("The file does not exist. Do you want to delete it"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
                if (m_allItemInfo.size() > currentIndex().row() && currentIndex().row() >= 0) {
                    QString path = m_allItemInfo[currentIndex().row()].path;
                    dApp->m_allPath.removeOne(path);
                    m_allItemInfo.removeAt(currentIndex().row());
                    refresh();
                } else if (currentIndex().row() < 0 && m_allItemInfo.size() > 0) {
                    if (QMessageBox::Yes == QMessageBox::information(NULL, tr("Delete!!"), tr("Delete all history imports ?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
                        m_allItemInfo.clear();
                        dApp->m_allPath.clear();
                        refresh();
                    }
                }
                emit dApp->saveSetting();
            }
        }

    }

}

void view::onClicked(const QModelIndex &index)
{
    qDebug() << index.row();
    m_currentModelIndex = index.row();
}

void view::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        QModelIndex i = m_model->item(m_model->rowCount() - 2, 0)->index();
        m_model->removeRow(i.row());
    }
    this->verticalScrollBar()->setValue(50);
    qDebug() << "-------------- ";
}



