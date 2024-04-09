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
#include <QMutex>
#include <QDir>

#include "data.h"

DWIDGET_USE_NAMESPACE

const QString PIC_DIR_PATH =   QDir::homePath() +
                               "/.config/fantascene-dynamic-wallpaper/.thumbnail";

#define dApp (static_cast<Application*>(QCoreApplication::instance()))
class Application : public DApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    const QString thumbnailCachePath();
    const QPixmap getThumbnail(const QString &path);

    void setDesktopTransparent();
    void setDesktopNoTransparent();

    bool setThumbnail(const QString &path);

    void setPlayListTimer(int s);
    void setisPlayList(bool bRet);

    bool addLocalPaths(QStringList strList);
    bool removeLocalPaths(QStringList strList);
    bool clearLocalPaths();

    bool addPlayListaths(QStringList strList);
    bool removePlayListPaths(QStringList strList);
    bool clearPlayListPaths();
Q_SIGNALS:
    void setPlayPath(const QString &PATH);
    void setPlayPath2(const QString &PATH);
    void setMpvPlay();
    void setMpvpause();
    void setMpvstop();
    void setMpvVolume(const qint32 volume);
    void setMpvValue(const QString &key, const QString &value);

    void setScreen(const int &index);
    void pathChanged(const QString &);
    void pathChanged2(const QString &);
    void refreshPix(const QPixmap &);
    void setScreenMode(const QString &);
    void sigupdateGeometry();
    void quitApp();

    void setWallPaper(const QString &path);
    void setWallPaper2(const QString &path);
    void saveSetting();

    void addPaperView(const QString &path);//加入全部播放列表

    void addplaylist(const QString &path);//加入自定义播放列表

    void removePaperView(const QString &path);

    void sigscreenshot();

    void moreSettingSave();

    void sigActiveWindow();

    void sigDesktopActive();

    void sigReadPlayerConfig();


public:
    QThread *m_startDesktop{nullptr};
    QProcess *m_startDesktopProcess{nullptr};
    qint64 m_processId{0};
    QRect m_manual{0, 0, 0, 0};
    ScreenMode m_cuurentMode{IdCopyScreen};
    int m_currentScreenNum{0};

    QStringList m_allPath;
    QStringList m_playlistPath;
    QMutex mutex;

    QMap <unsigned long, bool>m_x11WindowFuscreen;
    QVector <unsigned long> m_screenWid;
    bool m_isNoMpvPause{true};
    bool m_isPlayList{false};
    int m_PlaylistTimer{600}; //s
    QTimer *m_pplaylistTimer{nullptr};

    MoreSetData m_moreData;
//    int m_isAutoMode{0};
    QString m_currentPath;
    bool m_isPath2{false};  //是否副屏显示其他壁纸
    QString m_currentPath2; //副屏壁纸

    bool m_currentIsPlay{false};

    QString m_wallpaperEnginePath;
};

#endif // APPLICATION_H
