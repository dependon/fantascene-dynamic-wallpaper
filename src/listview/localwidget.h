#ifndef LOCALWIDGET_H
#define LOCALWIDGET_H

#include <QWidget>
class view;
class QFileSystemWatcher;
namespace Ui {
class LocalWidget;
}

class LocalWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LocalWidget(QWidget *parent = nullptr);
    ~LocalWidget();

    QStringList getAllpath();
    void watchDirectory(const QString &path, QFileSystemWatcher &watcher);
private Q_SLOTS:
    void on_playBtn_clicked();
    void on_directoryChanged(const QString &path);
    void on_fileChanged(const QString &path);

private:
    Ui::LocalWidget *ui;
    view *m_viewLocal{nullptr};
    QString m_strLocalPATH;
    QFileSystemWatcher * m_fileWacher{nullptr};
    QStringList m_allPath;
};

#endif // LOCALWIDGET_H
