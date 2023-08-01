#include "gioclass.h"

#include <glib.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>
#include <QDebug>
#include <QFileInfo>
#include <QDirIterator>
#include <QDir>
#include <QStandardPaths>
#include <QImageReader>

#define ICONSIZE 20

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
        qDebug() << "App没找到:" << appPath;
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
    qDebug()<<mimeType.iconName();
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

QIcon GioClass::getIcon(const QFileInfo &info)
{
    QIcon fileIcon;
    QMimeDatabase mimeDatabase;
    //image file, return a thumbnail.
    QPixmap tmpPixmap;
    QImageReader reader(info.filePath());
    if (reader.size().width() <= 4096 && reader.size().height() <= 4096) {
        tmpPixmap = QPixmap::fromImage(reader.read());
    }
    if (!tmpPixmap.isNull()) {
        tmpPixmap = tmpPixmap.scaled(ICONSIZE, ICONSIZE);
        fileIcon = QIcon(tmpPixmap);
        return fileIcon;
    }

    //we need do something special to '*.desktop' file.
    if (mimeDatabase.mimeTypeForFile(info).iconName() == QString("application-x-desktop")) {
        std::string tmp_str = info.filePath().toStdString();
        const char *file_path = tmp_str.c_str();
        GDesktopAppInfo *desktop_app_info = g_desktop_app_info_new_from_filename(file_path);
        if (desktop_app_info != nullptr) {
            char *tmp_icon_name = g_desktop_app_info_get_string(desktop_app_info, "Icon");
            QIcon desktopFileIcon = QIcon::fromTheme(QString(tmp_icon_name));
            if (desktopFileIcon.isNull()) {
                QPixmap pixmap = QPixmap(QString(tmp_icon_name));
                if (pixmap.isNull()) {
                    fileIcon = QIcon::fromTheme("application-x-desktop");
                } else {
                    pixmap = pixmap.scaled(ICONSIZE, ICONSIZE);
                    QIcon pixmapIcon = QIcon(pixmap);
                    fileIcon = pixmapIcon;
                }
            } else {
                fileIcon = desktopFileIcon;
            }
            if (tmp_icon_name != nullptr) {
                g_free(tmp_icon_name);
            }
        }
        return fileIcon;
    }
    QString filePath = info.filePath();
    std::string str = filePath.toStdString();
    const char *file_path = str.c_str();
    GFile *g_file = g_file_new_for_path(file_path);
    GFileInfo *file_info = g_file_query_info(g_file,
                                             G_FILE_ATTRIBUTE_STANDARD_ICON,
                                             G_FILE_QUERY_INFO_NONE,
                                             nullptr,
                                             nullptr);

    GIcon *g_icon = g_file_info_get_icon(file_info);
    const gchar *const *icon_names = g_themed_icon_get_names(G_THEMED_ICON(g_icon));
    QIcon icon;
    if (icon_names != nullptr) {
        QString iconName = QString(*icon_names);
        while (iconName.contains(".") || iconName.contains("/")) {
            qDebug() << *icon_names;
            icon_names++; //we need use second string, first string is not correct some times (for example, a wps-office-doc).
            iconName = QString(*icon_names);
        }
        icon = QIcon::fromTheme(iconName);
    }

    g_object_unref(file_info);
    g_object_unref(g_file);

    return icon;
}

QString GioClass::getDesktop2Name(const QString &strName)
{
    QString text = strName;
    if (!text.endsWith(QString(".desktop"))) {
        return text;
    }

    QString keyWord = "Name[" + QLocale::system().name() + "]";
    std::string tmp_key = keyWord.toStdString();

    QString filePath = strName;
    std::string tmp_str = filePath.toStdString();
    const char *file_path = tmp_str.c_str();
    GDesktopAppInfo *desktop_app_info = g_desktop_app_info_new_from_filename(file_path);

    if (desktop_app_info != nullptr) {
        char *tmp_name = g_desktop_app_info_get_string(desktop_app_info, tmp_key.c_str());
        if (tmp_name != nullptr) {
            text = QString(tmp_name);
            g_free(tmp_name);
        } else {
            tmp_name = g_desktop_app_info_get_string(desktop_app_info, "Name");
            if (tmp_name != nullptr) {
                text = QString(tmp_name);
                g_free(tmp_name);
            }
        }

        g_object_unref(desktop_app_info);
    }
    return text;
}
