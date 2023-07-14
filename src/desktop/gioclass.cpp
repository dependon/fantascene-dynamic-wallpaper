#include "gioclass.h"

#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include <QDebug>
#include <QFileInfo>
#include <QDirIterator>
#include <QDir>
#include <QStandardPaths>
GioClass::GioClass()
{

}

QString GioClass::getDefaultApp(const QString &mimeType)
{
    GAppInfo* defaultApp = g_app_info_get_default_for_type(mimeType.toLocal8Bit().constData(), FALSE);
    if(!defaultApp)
        return "";

    const char* desktop_id = g_app_info_get_id(defaultApp);
    GDesktopAppInfo* desktopAppInfo = g_desktop_app_info_new(desktop_id);
    if(!desktopAppInfo)
        return "";
    QString desktopFile = g_desktop_app_info_get_filename(desktopAppInfo);

    g_object_unref(defaultApp);
    g_object_unref(desktopAppInfo);

    return desktopFile;
}

bool GioClass::setDefautlApp(const QString &mimeType, const QString &appPath)
{
    GAppInfo* app = nullptr;
    GList* apps = nullptr;
    apps = g_app_info_get_all();
    GList* iterator = apps;
    while (iterator)
    {
        const char* desktop_id = g_app_info_get_id((GAppInfo*)iterator->data);
        GDesktopAppInfo* desktopAppInfo = g_desktop_app_info_new(desktop_id);
        if (desktopAppInfo)
        {
            if (appPath == g_desktop_app_info_get_filename(desktopAppInfo))
            {
                app = (GAppInfo*)iterator->data;
                g_object_unref(desktopAppInfo);
                break;
            }
            g_object_unref(desktopAppInfo);
        }
        if (appPath.endsWith("/" + QString::fromLocal8Bit(desktop_id)))
        {
            app = (GAppInfo*)iterator->data;
            break;
        }
        iterator = iterator->next;
    }
    g_list_free(apps);
    if (!app)
    {
        qDebug() << "没App找到:" << appPath;
        return false;
    }

    GError* error = nullptr;
    //xml类型的文件，需要同时设置application/xml和text/xml字段
    if (mimeType == "application/xml")
    {
        QString spMimeType = "text/xml";
        g_app_info_set_as_default_for_type(app,spMimeType.toLocal8Bit().constData(),&error);
    }
    g_app_info_set_as_default_for_type(app,mimeType.toLocal8Bit().constData(),&error);
    if (error) {
        qDebug () << "无法设置默认的程序:" << error->message;
        g_free(error);
        return false;
    }

    return true;
}

QStringList GioClass::getCanUseApps(const QMimeType &mimeType)
{
    return getCanUseApps(mimeType.iconName());
}


QStringList GioClass::getCanUseApps(const QString &mimeType)
{
    QStringList canUseApps;
    GList* AppInfoList = g_app_info_get_recommended_for_type(mimeType.toLocal8Bit().constData());
    GList* iterator = AppInfoList;

    while(iterator){
        GAppInfo* appInfo = (GAppInfo*)iterator->data;
        if(appInfo)
        {
            const char* desktopId = g_app_info_get_id(appInfo);
            GDesktopAppInfo* dekstopAppInfo = g_desktop_app_info_new(desktopId);
            QString app = g_desktop_app_info_get_filename(dekstopAppInfo);
            canUseApps << app;
            g_object_unref(dekstopAppInfo);
        }
        iterator = iterator->next;
    }
    g_list_free(AppInfoList);
    return canUseApps;
}

QStringList GioClass::getAllDesktopFiles()
{
      QStringList desktopFiles;

      for (QString desktopFolder : getAllApplicationsFolders()) {
          QDirIterator it(desktopFolder, QStringList("*.desktop"),
                          QDir::Files | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);
          while (it.hasNext()) {
            it.next();
            desktopFiles.append(it.filePath());
          }
      }
      return desktopFiles;
}

QStringList GioClass::getAllApplicationsFolders()
{
    QStringList desktopFolders;
    desktopFolders << QString("/usr/share/applications")
                   << QString("/usr/local/share/applications")
                   << QString("/usr/share/gnome/applications");
    for (const QString &path : QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation))
    {
      if (desktopFolders.contains(path))
          continue;
      desktopFolders.append(path);
    }

    return desktopFolders;
}
