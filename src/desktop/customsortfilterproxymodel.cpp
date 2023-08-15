#include "customsortfilterproxymodel.h"
#include <QModelIndex>
#include <QDateTime>
CustomSortFilterProxyModel::CustomSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool CustomSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QFileSystemModel *model = qobject_cast<QFileSystemModel*>(sourceModel());

    if (model) {
        // 获取左右索引对应的文件路径
        QString leftPath = model->filePath(left);
        QString rightPath = model->filePath(right);

        QFileInfo leftInfo(leftPath);
        QFileInfo rightInfo(rightPath);
        // 根据不同的排序规则进行比较
        switch (sortRole()) {
        case Qt::DisplayRole: // 按显示文本排序（名称）
            return QString::localeAwareCompare(leftInfo.fileName(),rightInfo.fileName()) < 0;
        case Qt::UserRole + 1: // 按修改时间排序
            return leftInfo.lastModified() < rightInfo.lastModified();
        case Qt::UserRole + 2: // 按文件大小排序
            return leftInfo.size() < rightInfo.size();
        case Qt::UserRole + 3: // 按文件类型排序
            return QString::localeAwareCompare(model->type(left), model->type(right)) < 0;
        default:
            break;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
