#ifndef TIMEDISPLAYWIDGET_H
#define TIMEDISPLAYWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QVBoxLayout>

class TimeDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    TimeDisplayWidget(QWidget *parent = nullptr);



public Q_SLOTS:
    // 设置字体的接口
    void setFontFamily(const QString &fontFamily);
    // 设置字体大小的接口
    void setFontPointSize(int size);
    // 设置字体颜色的接口
    void setFontColor(const QColor &color);
    // 设置时间控件位置
    void setTimeMove(int x,int y);
    // 设置时间显示格式
    void setTimeFormat(const QString &format);
    // 直接设置字体
    void setFont(const QFont & font);

private:
    QLabel *timeLabel{nullptr};
    QTimer *timer{nullptr};
    QString timeFormat;  // 用于定义时间格式

private Q_SLOTS:
    void updateTime();
};



#endif // TIMEDISPLAYWIDGET_H
