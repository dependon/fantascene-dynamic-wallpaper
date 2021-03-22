#ifndef APPLICATION_H
#define APPLICATION_H
#define protected public
#include <DApplication>
#undef protected
#include <QCoreApplication>
#include <QString>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QRect>

#include "data.h"
DWIDGET_USE_NAMESPACE

#define dApp (static_cast<Application*>(QCoreApplication::instance()))
class Application : public DApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    const QString thumbnailCachePath();
    const QPixmap getThumbnail(const QString &path);
Q_SIGNALS:
    void setPlayPath(const QString &PATH);
    void setMpvPlay();
    void setMpvpause();
    void setMpvstop();
    void setMpvVolume(const qint32 volume);
    void setScreen(const int &index);
    void pathChanged(const QString &);
    void refreshPix(const QPixmap &);
    void setScreenMode(const QString &);
    void sigupdateGeometry();
    void quitApp();

public:
    QThread *m_startDesktop{nullptr};
    QProcess *m_startDesktopProcess{nullptr};
    qint64 m_processId{0};
    QRect m_manual{0, 0, 0, 0};
    ScreenMode m_cuurentMode{IdCopyScreen};
    int m_currentScreenNum{0};


};

#endif // APPLICATION_H
