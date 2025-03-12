#include "cpumonitorwidget.h"
#include <QVBoxLayout>
#include "application.h"
#include "inimanager.h"
#include <QScreen>

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
    setMove(0,0);

    connect(dApp,&Application::setCpuFontColor,this,&CpuMonitorWidget::setFontColor);
    connect(dApp,&Application::setCpuMove,this,&CpuMonitorWidget::setMove);

    // read ini
    connect(dApp,&Application::setCpuFontColor,this,&CpuMonitorWidget::setFontColor);
    connect(dApp,&Application::setCpuMove,this,&CpuMonitorWidget::setMove);

    int FontX= IniManager::instance()->value("CpuDisplay/X",0).toInt();
    int FontY = IniManager::instance()->value("CpuDisplay/Y",0).toInt();

    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("CpuDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    this->setFontColor(color);
    this->setMove(FontX,FontY);
}

void CpuMonitorWidget::setMove(int x, int y)
{
    if( x == 0 && y == 0 )
    {
        if(qApp->screens().size()>0)
        {
            int x = qApp->screens().at(0)->geometry().width()-this->width()-400;
            int y = 100;
            this->move(x,y);

        }
    }
    else
    {
        this->move(x, y);
    }
}

void CpuMonitorWidget::setFontColor(const QColor &color)
{
    // 设置轮廓颜色和宽度
    if(m_areaSeries)
    {
        QPen pen(color);
        m_areaSeries->setPen(pen);
    }

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
