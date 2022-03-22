#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "mpvwidget.h"
#include "data.h"

#include <QWidget>
#include <QHBoxLayout>

#include "webwidget.h"
#include "desktop/iconview.h"
class QLabel;
//class Desktop;
class Wallpaper : public QWidget
{
    Q_OBJECT
public:
    explicit Wallpaper(QString path = nullptr, int currentScreen = 0, QWidget *parent = nullptr);

    void changeScreenMode(ScreenMode mode);
    void LeftMouseMove(QWidget *eventsReciverWidget, QPoint clickPos);
    void LeftMouseRelease(QWidget *eventsReciverWidget, QPoint clickPos);
    void LeftMousePress(QWidget *eventsReciverWidget, QPoint clickPos);

    void setIconVisble(bool visble);
public Q_SLOTS:
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
    void slotMouseClick(const int &index);

private:
    void registerDesktop();
    bool event(QEvent *event) override;

private Q_SLOTS:
    void onSysLockState(QString, QVariantMap key2value, QStringList);

    void slotSetMpvValue(const QString &key, const QString &value);

    void slotSetTransparency(const int value);
private:
    QHBoxLayout *mainlayout;
    QString m_currentPath;
    MpvWidget *m_mpv{nullptr};
    MpvWidget *m_mpv2{nullptr};
    int m_currentScreen{0};
    QLabel *m_label2{nullptr};

    webWidget *m_webView{nullptr};
    webWidget *m_webView2{nullptr};
    QTimer *m_mouseWebEventTimer{nullptr};
    QPoint m_currentPos{0, 0};
//    ScreenMode m_cuurentMode{IdCopyScreen};
//    Desktop *de{nullptr};
    IconView *m_iconView{nullptr};

};

#endif // WALLPAPER_H
