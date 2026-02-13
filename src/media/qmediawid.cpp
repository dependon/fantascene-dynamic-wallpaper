#include "qmediawid.h"
#include "ui_qmediawid.h"
#include "ini/inimanager.h"
#include <QLabel>
#include <QOpenGLWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QResizeEvent>
#if QT_VERSION_MAJOR == 6
#include <QAudioOutput>
#endif
QMediaWid::QMediaWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QMediaWid)
{

    ui->setupUi(this);

    m_player = new QMediaPlayer(this);


    setAttribute(Qt::WA_TranslucentBackground);

    videoItem = new QGraphicsVideoItem;
    scene = new QGraphicsScene;
    view = new QGraphicsView(scene);

    view->setStyleSheet("QGraphicsView { background: transparent; }");

    videoItem->setAspectRatioMode(Qt::IgnoreAspectRatio);
    scene->addItem(videoItem);

    videoItem->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    m_player->setVideoOutput(videoItem);
    ui->layout->addWidget(view);
#if QT_VERSION_MAJOR == 6
    // Qt 6：创建 QAudioOutput 并绑定到播放器
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
#endif
    if(IniManager::instance()->contains("WallPaper/voiceVolume"))
    {
        int iVulume =  IniManager::instance()->value("WallPaper/voiceVolume").toInt();
        setVolume(iVulume);
    }

    //核心：监听媒体状态变化，实现循环播放
    QObject::connect(m_player, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
        // 当媒体播放到末尾时，重置播放位置并重新播放
        if (status == QMediaPlayer::EndOfMedia) {
            // 重置播放位置到 0 毫秒（起始点）
            m_player->setPosition(0);
            // 重新开始播放
            m_player->play();
        }
    });


}

QMediaWid::~QMediaWid()
{
    m_player->stop();
    delete ui;
}

void QMediaWid::setFile(const QString &path)
{
#if QT_VERSION_MAJOR == 5
    m_player->setMedia(QUrl::fromLocalFile(path));
#else
    m_player->setSource(QUrl::fromLocalFile(path));
#endif
}

void QMediaWid::play()
{
    m_player->play();
}

void QMediaWid::stop()
{
    m_player->stop();
}

void QMediaWid::pause()
{
    m_player->pause();
}

void QMediaWid::setVolume(int value)
{
#if QT_VERSION_MAJOR == 5
    m_player->setVolume(value);
#else \
    // Qt 6 音量范围是 0.0 ~ 1.0（浮点型）
    if(m_audioOutput)
    {
        m_audioOutput->setVolume(0.8); // 80% 音量
    }
#endif

}

void QMediaWid::resizeEvent(QResizeEvent *event)
{
    videoItem->setSize(event->size());
    QWidget::resizeEvent(event);
}
