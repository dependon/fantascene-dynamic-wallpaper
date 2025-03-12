#ifndef NETWORKMONITORWIDGET_H
#define NETWORKMONITORWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "systemmonitor.h"

class NetworkMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkMonitorWidget(QWidget *parent = nullptr);

private Q_SLOTS:
    void updateUploadSpeed(const QVector<double>& speed);
    void updateDownloadSpeed(const QVector<double>& speed);

    // 设置控件位置
    void setMove(int x, int y);
    // 设置上行颜色的接口
    void setFontUpColor(const QColor &color);
    //设置下行颜色的接口
    void setFontDownColor(const QColor &color);
    //设置控件大小
    void setSize(int width,int height);

private:
    QLabel *uploadLabel;
    QLabel *downloadLabel;
    SystemMonitor *monitor;
};

#endif // NETWORKMONITORWIDGET_H
