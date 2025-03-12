#include "networkmonitorwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include "application.h"
#include "inimanager.h"
#include <QScreen>

NetworkMonitorWidget::NetworkMonitorWidget(QWidget *parent) : QWidget(parent)
{
    // setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    // setAttribute(Qt::WA_TranslucentBackground);

    monitor = SystemMonitor::getInstance();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    uploadLabel = new QLabel("↑ 0.0 KB/s", this);
    //uploadLabel->setStyleSheet("color: #00ff00; font: 9pt 'Microsoft YaHei';");
    downloadLabel = new QLabel("↓ 0.0 KB/s", this);
    //downloadLabel->setStyleSheet("color: #00ffff; font: 9pt 'Microsoft YaHei';");

    layout->addWidget(uploadLabel);
    layout->addWidget(downloadLabel);

    setMove(0,0);
    connect(monitor, &SystemMonitor::uploadSpeedChanged, this, &NetworkMonitorWidget::updateUploadSpeed);
    connect(monitor, &SystemMonitor::downloadSpeedChanged, this, &NetworkMonitorWidget::updateDownloadSpeed);

    // read ini
    connect(dApp,&Application::setNetworkUpFontColor,this,&NetworkMonitorWidget::setFontUpColor);
    connect(dApp,&Application::setNetworkDownFontColor,this,&NetworkMonitorWidget::setFontDownColor);
    connect(dApp,&Application::setNetworkMove,this,&NetworkMonitorWidget::setMove);
    connect(dApp,&Application::setNetworkSize,this,&NetworkMonitorWidget::setSize);

    int FontX= IniManager::instance()->value("NetworkDisplay/X",0).toInt();
    int FontY = IniManager::instance()->value("NetworkDisplay/Y",0).toInt();
    int width= IniManager::instance()->value("NetworkDisplay/Width",400).toInt();
    int height = IniManager::instance()->value("NetworkDisplay/Height",260).toInt();


    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("NetworkDisplay/Color_Up",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    this->setFontUpColor(color);

    color = QColor(170,255,255);
    variantColor = IniManager::instance()->value("NetworkDisplay/Color_Down",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }

    this->setFontDownColor(color);
    this->setMove(FontX,FontY);
    this->setSize(width,height);

    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void NetworkMonitorWidget::setMove(int x, int y)
{
    if( x <= 0 && y <= 0 )
    {
        if(qApp->screens().size()>0)
        {
            int x = qApp->screens().at(0)->geometry().width()-this->width()-200;
            int y = qApp->screens().at(0)->geometry().height()-this->height()-250;;
            this->move(x,y);

        }
    }
    else
    {
        this->move(x, y);
    }
}

void NetworkMonitorWidget::setFontUpColor(const QColor &color)
{
    if(uploadLabel)
    {
        // 创建一个 QPalette 对象
        QPalette palette = uploadLabel->palette();

        // 设置文本颜色为绿色
        palette.setColor(QPalette::WindowText, color);

        // 将修改后的调色板应用到标签上
        uploadLabel->setPalette(palette);
    }
}

void NetworkMonitorWidget::setFontDownColor(const QColor &color)
{
    if(downloadLabel)
    {
        // 创建一个 QPalette 对象
        QPalette palette = downloadLabel->palette();

        // 设置文本颜色为绿色
        palette.setColor(QPalette::WindowText, color);

        // 将修改后的调色板应用到标签上
        downloadLabel->setPalette(palette);
    }
}

void NetworkMonitorWidget::setSize(int width, int height)
{
    if(width>0 && height > 0)
    {
        resize(width,height);
    }
    else
    {
        resize(100,60);
    }
}

void NetworkMonitorWidget::updateUploadSpeed(const QVector<double>& speed)
{
    double current = speed.isEmpty() ? 0 : speed.last();
    QString unit = current >= 1024 ? "MB/s" : "KB/s";
    double value = current >= 1024 ? current/1024 : current;
    uploadLabel->setText(QString("↑ %1 %2").arg(value, 0, 'f', 1).arg(unit));
}

void NetworkMonitorWidget::updateDownloadSpeed(const QVector<double>& speed)
{
    double current = speed.isEmpty() ? 0 : speed.last();
    QString unit = current >= 1024 ? "MB/s" : "KB/s";
    double value = current >= 1024 ? current/1024 : current;
    downloadLabel->setText(QString("↓ %1 %2").arg(value, 0, 'f', 1).arg(unit));
}
