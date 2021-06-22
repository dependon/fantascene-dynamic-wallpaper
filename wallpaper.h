#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "mpvwidget.h"
#include "data.h"

#include <QWidget>
#include <QHBoxLayout>

#include "webwidget.h"

class QLabel;
class Wallpaper : public QWidget
{
    Q_OBJECT
public:
    explicit Wallpaper(QString path = nullptr, int currentScreen = 0, QWidget *parent = nullptr);

    void changeScreenMode(ScreenMode mode);
    void LeftMouseClick(QWidget *eventsReciverWidget, QPoint clickPos);
public slots:
    void setFile(const QString &path);
    void setVolume(const qint32 volume);
    void setScreen(const int &index);
    void clear();
    void play();
    void pause();
    void stop();

    void slotrefreshPix(const QPixmap &pix);
    void slotsetScreenMode(const QString &mode);
    void updateGeometry();

    void slotMouseEvent();
private:
    void registerDesktop();
    bool event(QEvent *event) override;
private slots:
    void onSysLockState(QString, QVariantMap key2value, QStringList);

    void slotSetMpvValue(const QString &key, const QString &value);
private:
    QHBoxLayout *mainlayout;
    QString m_currentPath;
    MpvWidget *m_mpv{nullptr};

    int m_currentScreen{0};
    QLabel *m_label2{nullptr};
    webWidget *m_webView{nullptr};
    webWidget *m_webView2{nullptr};
    QTimer *m_mouseWebEventTimer{nullptr};
    QPoint m_currentPos{0, 0};
//    ScreenMode m_cuurentMode{IdCopyScreen};
};

#endif // WALLPAPER_H
