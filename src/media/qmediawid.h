#ifndef QMEDIAWID_H
#define QMEDIAWID_H

#include <QWidget>
#include <QMediaPlayer>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsVideoItem;
#if QT_VERSION_MAJOR == 6
class QAudioOutput;
#endif
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

    QGraphicsVideoItem* videoItem{nullptr};
    QGraphicsScene* scene{nullptr};
    QGraphicsView* view{nullptr};
#if QT_VERSION_MAJOR == 6
    QAudioOutput *m_audioOutput = nullptr;
#endif
};

#endif // QMEDIAWID_H
