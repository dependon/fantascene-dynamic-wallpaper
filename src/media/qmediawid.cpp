#include "qmediawid.h"
#include "ui_qmediawid.h"
#include "ini/inimanager.h"
#include <QLabel>
#include <QOpenGLWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QResizeEvent>

QMediaWid::QMediaWid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QMediaWid)
{

    ui->setupUi(this);

    m_player = new QMediaPlayer(this);

    m_playlist = new QMediaPlaylist(this);

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

    m_player->setPlaylist(m_playlist);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);

    if(IniManager::instance()->contains("WallPaper/voiceVolume"))
    {
        int iVulume =  IniManager::instance()->value("WallPaper/voiceVolume").toInt();
        setVolume(iVulume);
    }



}

QMediaWid::~QMediaWid()
{
    m_player->stop();
    delete ui;
}

void QMediaWid::setFile(const QString &path)
{
    m_playlist->clear();
    m_playlist->addMedia(QUrl::fromLocalFile(path));
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
    m_player->setVolume(value);
}

void QMediaWid::resizeEvent(QResizeEvent *event)
{
    videoItem->setSize(event->size());
    QWidget::resizeEvent(event);
}
