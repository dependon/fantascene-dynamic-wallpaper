#include "cpumonitorwidget.h"
#include <QVBoxLayout>
CpuMonitorWidget::CpuMonitorWidget(QWidget *parent) : QWidget(parent)
{
    m_monitor = SystemMonitor::getInstance();

//    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
//    setAttribute(Qt::WA_TranslucentBackground);


    // 创建 CPU 图表
    m_cpuChart = new QChart();
    m_cpuChart->setTitle(QString(tr("CPU: %1").arg(m_monitor->getCpuModel())));
    m_cpuChart->legend()->hide();
    // 确保图表背景透明
    m_cpuChart->setBackgroundVisible(false);
    m_cpuChart->setPlotAreaBackgroundVisible(false);

    m_cpuSeries = new QLineSeries();
    //cpuChart->addSeries(cpuSeries);
    m_cpuSeries->append(0, 1);
    m_cpuSeries->append(0, 2);
    // 创建面积系列，下边界默认为 0
    m_areaSeries = new QAreaSeries(m_cpuSeries);
    m_cpuChart->addSeries(m_areaSeries);

    m_cpuAxisX = new QValueAxis();
    m_cpuAxisX->setRange(0, 60);
    m_cpuAxisX->setLabelFormat("%d");
    m_cpuAxisY = new QValueAxis();
    m_cpuAxisY->setRange(0, 100);

    m_cpuChart->addAxis(m_cpuAxisX, Qt::AlignBottom);
    m_cpuChart->addAxis(m_cpuAxisY, Qt::AlignLeft);


    m_areaSeries->attachAxis(m_cpuAxisX);
    m_areaSeries->attachAxis(m_cpuAxisY);


    m_cpuChartView = new QChartView(m_cpuChart);
    m_cpuChartView->setRenderHint(QPainter::Antialiasing);
    // 设置图表视图背景透明
    m_cpuChartView->setBackgroundBrush(Qt::transparent);
    m_cpuChartView->setFrameShape(QFrame::NoFrame);
    // 确保图表视图的视口背景透明
    m_cpuChartView->viewport()->setAutoFillBackground(false);
    m_cpuChartView->viewport()->setStyleSheet("background-color: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_cpuChartView);

    connect(m_monitor, &SystemMonitor::cpuUsageChanged, this, &CpuMonitorWidget::updateCpuUsage);
}

void CpuMonitorWidget::updateCpuUsage(const QVector<double> &usage)
{
    m_cpuSeries->clear();
    for (int i = 0; i < usage.size(); ++i) {
        m_cpuSeries->append(i, usage[i]);
    }
    if(usage.size() > 0)
    {
        QString str = QString::number(usage.at(usage.size()-1),'f',1)+"%";
        m_cpuAxisX->setTitleText(str);
    }
}
