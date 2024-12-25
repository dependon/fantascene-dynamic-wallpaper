#ifndef MEDIAPLUGINWIDGET_H
#define MEDIAPLUGINWIDGET_H

#include <QWidget>
#include "qmediawid.h"
#include "mpvwidget.h"
#include "application.h"

namespace Ui {
class MediaPluginWidget;
}

class MediaPluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPluginWidget(QWidget *parent = nullptr);
    ~MediaPluginWidget();
    void setFile(const QString & path);
    void play();         //播放
    void stop();       //停止
    void pause();      //暂停
    void setVolume(int value); //音量

    // mpv特有
    void setAspect(double value); //设置比例
    void setProperty(const QString &name, const QVariant &value);
    QVariant getProperty(const QString &name) const;

private Q_SLOTS:
    void setType(VideoWidgetType type);
private:
    Ui::MediaPluginWidget *ui;
    VideoWidgetType m_currentType{Type_Mpv};
    MpvWidget * m_mpv{nullptr};
    QMediaWid * m_qmedia{nullptr};

    QString m_currentVideo;

};

#endif // MEDIAPLUGINWIDGET_H
