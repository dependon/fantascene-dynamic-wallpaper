#ifndef CPUMONITORWIDGET_H
#define CPUMONITORWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QAreaSeries>
#include "systemmonitor.h"

QT_CHARTS_USE_NAMESPACE

class CpuMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CpuMonitorWidget(QWidget *parent = nullptr);
private Q_SLOTS:
    void updateCpuUsage(const QVector<double>& usage);

    // 设置控件位置
    void setMove(int x, int y);
    // 设置字体颜色的接口
    void setFontColor(const QColor &color);

private:
    QChart *m_cpuChart{nullptr};
    QLineSeries *m_cpuSeries{nullptr};
    QValueAxis *m_cpuAxisX{nullptr};
    QValueAxis *m_cpuAxisY{nullptr};
    QChartView *m_cpuChartView{nullptr};
    QAreaSeries *m_areaSeries{nullptr};
    SystemMonitor *m_monitor{nullptr};
};

#endif // CPUMONITORWIDGET_H
