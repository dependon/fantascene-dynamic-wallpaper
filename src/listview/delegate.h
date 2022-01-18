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
