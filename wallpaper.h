#ifndef WALLPAPER_H
#define WALLPAPER_H

#include "mpvwidget.h"
#include "data.h"

#include <QWidget>
#include <QHBoxLayout>

class Wallpaper : public QWidget
{
    Q_OBJECT
public:
    explicit Wallpaper(QString path = nullptr, int currentScreen = 0, QWidget *parent = nullptr);

    void changeScreenMode(ScreenMode mode);
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
private:
    void registerDesktop();
private slots:
    void onSysLockState(QString, QVariantMap key2value, QStringList);

    void slotSetMpvValue(const QString &key, const QString &value);
private:
    QHBoxLayout *mainlayout;
    QString m_currentPath;
    MpvWidget *m_mpv{nullptr};

    int m_currentScreen{0};
    QLabel *m_label2{nullptr};
//    ScreenMode m_cuurentMode{IdCopyScreen};
};

#endif // WALLPAPER_H
