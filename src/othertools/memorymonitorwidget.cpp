#include "memorymonitorwidget.h"
#include <QVBoxLayout>
#include <QAreaSeries>
#include "application.h"
#include "inimanager.h"
#include <QScreen>

MemoryMonitorWidget::MemoryMonitorWidget(QWidget *parent) : QWidget(parent)
{
    m_monitor = SystemMonitor::getInstance();

//    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
//    setAttribute(Qt::WA_TranslucentBackground);

    // 创建内存图表
    m_memoryChart = new QChart();
    m_memoryChart->setTitle(QString(tr("Memory(%1)").arg(m_monitor->getMemoryInfo())));
    m_memoryChart->legend()->hide();
    // 确保图表背景透明
    m_memoryChart->setBackgroundVisible(false);
    m_memoryChart->setPlotAreaBackgroundVisible(false);

    m_memorySeries = new QLineSeries();
    //memoryChart->addSeries(memorySeries);
    m_memorySeries->append(0, 1);
    m_memorySeries->append(0, 2);
    // 创建面积系列，下边界默认为 0
    m_areaSeries = new QAreaSeries(m_memorySeries);
    m_memoryChart->addSeries(m_areaSeries);

    m_memoryAxisX = new QValueAxis();
    m_memoryAxisX->setRange(0, 60);
    m_memoryAxisX->setLabelFormat("%d");
    m_memoryAxisX->setTitleText(tr("Time (s)"));
    m_memoryAxisY = new QValueAxis();
    m_memoryAxisY->setRange(0, 100);

    m_memoryChart->addAxis(m_memoryAxisX, Qt::AlignBottom);
    m_memoryChart->addAxis(m_memoryAxisY, Qt::AlignLeft);


    m_areaSeries->attachAxis(m_memoryAxisX);
    m_areaSeries->attachAxis(m_memoryAxisY);

    m_memoryChartView = new QChartView(m_memoryChart);
    m_memoryChartView->setRenderHint(QPainter::Antialiasing);
    // 设置图表视图背景透明
    m_memoryChartView->setBackgroundBrush(Qt::transparent);
    m_memoryChartView->setFrameShape(QFrame::NoFrame);
    // 确保图表视图的视口背景透明
    m_memoryChartView->viewport()->setAutoFillBackground(false);
    m_memoryChartView->viewport()->setStyleSheet("background-color: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_memoryChartView);

    connect(m_monitor, &SystemMonitor::memoryUsageChanged, this, &MemoryMonitorWidget::updateMemoryUsage);
    connect(m_monitor, &SystemMonitor::memoryUsageChanged, this, &MemoryMonitorWidget::updateMemoryTitle);

    setMove(0,0);

    // read ini
    connect(dApp,&Application::setMemoryFontColor,this,&MemoryMonitorWidget::setFontColor);
    connect(dApp,&Application::setMemoryMove,this,&MemoryMonitorWidget::setMove);

    int FontX= IniManager::instance()->value("MemoryDisplay/X",0).toInt();
    int FontY = IniManager::instance()->value("MemoryDisplay/Y",0).toInt();

    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("MemoryDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    this->setFontColor(color);
    this->setMove(FontX,FontY);

}

void MemoryMonitorWidget::setMove(int x, int y)
{
    if( x == 0 && y == 0 )
    {
        if(qApp->screens().size()>0)
        {
            int x = qApp->screens().at(0)->geometry().width()-this->width()-400;
            int y = 400;
            this->move(x,y);

        }
    }
    else
    {
        this->move(x, y);
    }
}

void MemoryMonitorWidget::setFontColor(const QColor &color)
{
    // 设置轮廓颜色和宽度
    if(m_areaSeries)
    {
        QPen pen(color);
        m_areaSeries->setPen(pen);
    }
}

void MemoryMonitorWidget::updateMemoryUsage(const QVector<double> &usage)
{
    m_memorySeries->clear();
    for (int i = 0; i < usage.size(); ++i) {
        m_memorySeries->append(i, usage[i]);
        if(usage.size() > 0)
        {
            QString str = QString::number(usage.at(usage.size()-1),'f',1)+"%";
            m_memoryAxisX->setTitleText(str);
        }
    }
}

void MemoryMonitorWidget::updateMemoryTitle()
{
    m_memoryChart->setTitle(QString("Memory (%1)").arg(m_monitor->memoryInfo));
}
