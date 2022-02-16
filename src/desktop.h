#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>
#include <QIcon>
#include <QFileInfoList>
#include <QMenu>

namespace Ui {
class Desktop;
}

class Desktop : public QWidget
{
    Q_OBJECT

public:
    explicit Desktop(QWidget *parent = nullptr);
    ~Desktop();

    void initMenu();
    void addList();
    QIcon getIcon(const QString &path);

    QString readSettings(const QString &path, const QString &group, const QString &key);

    void open(const QString &newpath);
private Q_SLOTS:
    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::Desktop *ui;

    QFileInfoList m_infolist;
    QMenu *m_rightMenu{nullptr};
};

#endif // DESKTOP_H
