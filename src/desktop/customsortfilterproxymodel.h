#ifndef CUSTOMSORTFILTERPROXYMODEL_H
#define CUSTOMSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
class CustomSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    CustomSortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

};

#endif // CUSTOMSORTFILTERPROXYMODEL_H
