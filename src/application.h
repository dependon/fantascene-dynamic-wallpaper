#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QObject>
#include <QThread>
#include <QProcess>
#include <QRect>
#include <QMutex>
#include <QDir>
#include <QVector>

#include "data.h"



const QString PIC_DIR_PATH =   QDir::homePath() +
                               "/.config/fantascene-dynamic-wallpaper/.thumbnail";

#define dApp (static_cast<Application*>(QCoreApplication::instance()))
class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    const QString thumbnailCachePath();
    const QPixmap getThumbnail(const QString &path);

    void setDesktopTransparent();

    bool setThumbnail(const QString &path);
Q_SIGNALS:
    void setPlayPath(const QString &PATH);
    void setMpvPlay();
    void setMpvpause();
    void setMpvstop();
    void setMpvVolume(const qint32 volume);
    void setMpvValue(const QString &key, const QString &value);

    void setScreen(const int &index);
    void pathChanged(const QString &);
    void refreshPix(const QPixmap &);
    void setScreenMode(const QString &);
    void sigupdateGeometry();
    void quitApp();

    void setWallPaper(const QString &path);
    void saveSetting();

    void addPaperView(const QString &path);
    void removePaperView(const QString &path);

    void sigscreenshot();

    void moreSettingSave();

    void sigActiveWindow();

    void sigDesktopActive();

    void sigSetTransparency(const int value);

    void sigReadConfig();
public:
    QThread *m_startDesktop{nullptr};
    QProcess *m_startDesktopProcess{nullptr};
    qint64 m_processId{0};
    QRect m_manual{0, 0, 0, 0};
    ScreenMode m_cuurentMode{IdCopyScreen};
    int m_currentScreenNum{0};

    QStringList m_allPath;
    QMutex mutex;

    QMap <unsigned long, bool>m_x11WindowFuscreen;
    QVector <unsigned long> m_screenWid;
    bool m_isNoMpvPause{true};

    MoreSetData m_moreData;
//    int m_isAutoMode{0};
    QString m_currentPath;

    bool m_currentIsPlay{false};


    QString m_wallpaperEnginePath;
};

#endif // APPLICATION_H
