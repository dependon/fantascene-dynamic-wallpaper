#ifndef QMEDIAWID_H
#define QMEDIAWID_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
class QGraphicsView;
class QGraphicsScene;
class QGraphicsVideoItem;

namespace Ui {
class QMediaWid;
}

class QMediaWid : public QWidget
{
    Q_OBJECT

public:
    explicit QMediaWid(QWidget *parent = nullptr);
    ~QMediaWid();

    void setFile(const QString & path);
    void play();         //播放
    void stop();       //停止
    void pause();      //暂停

    void setVolume(int value); //音量
    void resizeEvent(QResizeEvent *event);

private:
    Ui::QMediaWid *ui;
    QMediaPlayer *m_player{nullptr};
    QMediaPlaylist* m_playlist{nullptr};

    QGraphicsVideoItem* videoItem{nullptr};
    QGraphicsScene* scene{nullptr};
    QGraphicsView* view{nullptr};
};

#endif // QMEDIAWID_H
