#ifndef APPLICATION_H
#define APPLICATION_H
#include <DApplication>
#include <QCoreApplication>
#include <QString>
#include <QObject>


DWIDGET_USE_NAMESPACE

#define dApp (static_cast<Application*>(QCoreApplication::instance()))
class Application : public DApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

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


};

#endif // APPLICATION_H
