#include "mediapluginwidget.h"
#include "ui_mediapluginwidget.h"
#include "ini/inimanager.h"

MediaPluginWidget::MediaPluginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaPluginWidget)
{
    ui->setupUi(this);
    qRegisterMetaType<VideoWidgetType>("VideoWidgetType");
    VideoWidgetType type = VideoWidgetType(IniManager::instance()->value("Media/CurrentPlugin",0).toInt());
    setType(type);
    connect(dApp,&Application::sigSetMeidaType,this,&MediaPluginWidget::setType);
}

MediaPluginWidget::~MediaPluginWidget()
{
    delete ui;
}

void MediaPluginWidget::setType(VideoWidgetType type)
{
    if (type == Type_Mpv)
    {
        if(m_mpv)
        {
            delete m_mpv;
            m_mpv = nullptr;
        }
        if(m_qmedia)
        {
            delete m_qmedia;
            m_qmedia = nullptr;
        }
        m_mpv = new MpvWidget(this);
        ui->layout->addWidget(m_mpv);
    }
    else if (type == Type_QMedia)
    {
        if(m_mpv)
        {
            delete m_mpv;
            m_mpv = nullptr;
        }
        if(m_qmedia)
        {
            delete m_qmedia;
            m_qmedia = nullptr;
        }
        m_qmedia = new QMediaWid(this);
        ui->layout->addWidget(m_qmedia);
    }
    m_currentType = type;
    if(!m_currentVideo.isEmpty())
    {
        setFile(m_currentVideo);
    }
}

void MediaPluginWidget::setFile(const QString &path)
{
    m_currentVideo = path;
    if (m_currentType == Type_Mpv)
    {
        if(m_mpv)
            m_mpv->command(QStringList() << "loadfile" << path);
    }
    else if (m_currentType == Type_QMedia)
    {
        if(m_qmedia)
        {
            m_qmedia->setFile(path);
            m_qmedia->play();
        }
    }
}

void MediaPluginWidget::play()
{
    if (m_currentType == Type_Mpv)
    {
        if(m_mpv)
            m_mpv->setProperty("pause", false);
    }
    else if (m_currentType == Type_QMedia)
    {
        if(m_qmedia)
            m_qmedia->play();
    }
}

void MediaPluginWidget::stop()
{
    if (m_currentType == Type_Mpv)
    {
        if(m_mpv)
            m_mpv->setProperty("stop", true);
    }
    else if (m_currentType == Type_QMedia)
    {
        if(m_qmedia)
            m_qmedia->stop();
    }
}

void MediaPluginWidget::pause()
{
    if (m_currentType == Type_Mpv)
    {
        if(m_mpv)
            m_mpv->setProperty("pause", true);
    }
    else if (m_currentType == Type_QMedia)
    {
        if(m_qmedia)
            m_qmedia->pause();
    }
}

void MediaPluginWidget::setVolume(int value)
{
    if (m_currentType == Type_Mpv)
    {
        if(m_mpv)
        {
            m_mpv->setProperty("volume", value);
        }
    }
    else if (m_currentType == Type_QMedia)
    {
        if(m_qmedia)
        {
            m_qmedia->setVolume(value);
        }

    }
}

void MediaPluginWidget::setAspect(double value)
{
    if(m_mpv)
    {
        m_mpv->setProperty("video-aspect", value);
        m_mpv->setProperty("video-aspect-override", value);
    }

}

void MediaPluginWidget::setProperty(const QString &name, const QVariant &value)
{
    if(m_mpv)
    {
        m_mpv->setProperty(name, value);
    }
}

QVariant MediaPluginWidget::getProperty(const QString &name) const
{
    QVariant reValue;
    if(m_mpv)
    {
        reValue = m_mpv->getProperty(name);
    }
    return reValue;
}
