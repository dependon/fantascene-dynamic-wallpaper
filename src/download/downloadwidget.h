#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
namespace Ui {
class DownloadWidget;
}

class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = nullptr);
    ~DownloadWidget();

private Q_SLOTS:
    void on_loadBtn_clicked();

private:
    Ui::DownloadWidget *ui;
};

#endif // DOWNLOADWIDGET_H
