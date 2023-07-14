#ifndef GIOCLASS_H
#define GIOCLASS_H

#include <QString>
#include <QMimeType>
#include <QMimeDatabase>

class GioClass
{
public:
    GioClass();
    //获得默认程序
    static QString getDefaultApp(const QString &mimeType);
    //设置默认程序
    static bool setDefautlApp(const QString &mimeType, const QString &appPath);
    //获得契合的可作为默认程序
    static QStringList getCanUseApps(const QMimeType &mimeType);
    static QStringList getCanUseApps(const QString &mimeType);
    static QStringList getAllDesktopFiles();
    static QStringList getAllApplicationsFolders();
};

#endif // GIOCLASS_H
