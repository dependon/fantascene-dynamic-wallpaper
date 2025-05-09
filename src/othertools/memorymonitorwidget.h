#ifndef MEMORYMONITORWIDGET_H
#define MEMORYMONITORWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAreaSeries>
#include "systemmonitor.h"

QT_CHARTS_USE_NAMESPACE

class MemoryMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MemoryMonitorWidget(QWidget *parent = nullptr);

private Q_SLOTS:
    void updateMemoryUsage(const QVector<double>& usage);
    void updateMemoryTitle();

    // 设置控件位置
    void setMove(int x, int y);
    // 设置字体颜色的接口
    void setColor(const QColor &color);
    void setFontColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

    void setSize(int width,int height);

private:
    QChart *m_memoryChart{nullptr};
    QLineSeries *m_memorySeries{nullptr};
    QValueAxis *m_memoryAxisX{nullptr};
    QValueAxis *m_memoryAxisY{nullptr};
    QChartView *m_memoryChartView{nullptr};
    QAreaSeries *m_areaSeries{nullptr};
    SystemMonitor *m_monitor{nullptr};


};

#endif // MEMORYMONITORWIDGET_H
