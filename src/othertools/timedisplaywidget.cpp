#include "timedisplaywidget.h"
#include "application.h"
#include "inimanager.h"
#include <QScreen>
TimeDisplayWidget::TimeDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    connect(dApp,&Application::setTimeFontFamily,this,&TimeDisplayWidget::setFontFamily);
    connect(dApp,&Application::setTimeFontPointSize,this,&TimeDisplayWidget::setFontPointSize);
    connect(dApp,&Application::setTimeFontColor,this,&TimeDisplayWidget::setFontColor);
    connect(dApp,&Application::setTimeMove,this,&TimeDisplayWidget::setTimeMove);
    connect(dApp,&Application::setTimeFormat,this,&TimeDisplayWidget::setTimeFormat);


    timeLabel = new QLabel(this);
    timeLabel->setAlignment(Qt::AlignLeft);

    // 创建定时器，每1秒触发一次更新时间操作
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TimeDisplayWidget::updateTime);
    timer->start(1000);

    // 简单的布局设置，将label添加到垂直布局中（可按需更改布局）
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(timeLabel);
    setLayout(layout);
    layout->setStretchFactor(timeLabel, 1);

    QSizePolicy policy = timeLabel->sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    timeLabel ->setSizePolicy(policy);

    // 初始更新时间显示
    updateTime();

    QString FontName = IniManager::instance()->value("TimeDisplay/FontName","Moonstar").toString();
    int FontSize = IniManager::instance()->value("TimeDisplay/FontSize",25).toInt();
    int FontX= IniManager::instance()->value("TimeDisplay/X",0).toInt();
    int FontY = IniManager::instance()->value("TimeDisplay/Y",0).toInt();

    timeFormat = IniManager::instance()->value("TimeDisplay/TimeFormat","yyyy-MM-dd HH:mm:ss").toString();

    QColor color = QColor(170,255,255);
    QVariant variantColor = IniManager::instance()->value("TimeDisplay/Color",color);
    if (variantColor.canConvert<QColor>()) {
        color = variantColor.value<QColor>();
    } else {
        color = QColor(170,255,255);
    }
    this->setFontFamily(FontName);
    this->setFontPointSize(FontSize);
    this->setFontColor(color);
    setTimeMove(FontX,FontY);

    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void TimeDisplayWidget::setFont(const QFont &font)
{
    timeLabel->setFont(font);
}

// 设置字体的接口实现
void TimeDisplayWidget::setFontFamily(const QString &fontFamily)
{
    QFont currentFont = timeLabel->font();
    currentFont.setFamily(fontFamily);
    timeLabel->setFont(currentFont);
}

// 设置字体大小的接口实现
void TimeDisplayWidget::setFontPointSize(int size)
{
    QFont currentFont = timeLabel->font();
    currentFont.setPointSize(size);
    timeLabel->setFont(currentFont);
    this->setMinimumSize(size*17,size*3);
    //timeLabel->setMinimumSize(size*14,size*2);
}

// 设置字体颜色的接口实现
void TimeDisplayWidget::setFontColor(const QColor &color)
{
    QPalette palette = timeLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    timeLabel->setPalette(palette);
}

void TimeDisplayWidget::setTimeMove(int x, int y)
{
    if( x == 0 && y == 0 )
    {
        if(qApp->screens().size()>0)
        {
            int x = qApp->screens().at(0)->geometry().width()-this->width()-100;
            int y = qApp->screens().at(0)->geometry().height()-this->height()-100;
            this->move(x,y);

        }
    }
    else
    {
        this->move(x, y);
    }
}

void TimeDisplayWidget::setTimeFormat(const QString &format)
{
    timeFormat = format;
}

void TimeDisplayWidget::updateTime()
{
    // 获取当前时间并按照设定格式格式化
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString formattedTime = currentDateTime.toString(timeFormat);
    timeLabel->setText(formattedTime);
}
