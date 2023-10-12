#include "dbmanager.h"
#include <QDir>
#include <QMutexLocker>
const QString SQL_PATH =   QDir::homePath() +
                           "/.config/fantascene-dynamic-wallpaper/fantascene.db";
const QString SQL_DIR =   QDir::homePath() +
                           "/.config/fantascene-dynamic-wallpaper/";

DBManager *DBManager::dbmanager = nullptr;
DBManager *DBManager::instance()
{
    if(!dbmanager)
    {
        dbmanager = new DBManager();
    }
    return dbmanager;
}

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    QFile file(SQL_PATH);
    if (file.exists()) {
        qDebug() << "file exists !!!";
    } else {
        qDebug() << "file no exists,mkdir file!!!.";
        QDir a;
        a.mkdir(SQL_DIR);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }
    m_db.setDatabaseName(SQL_PATH);
    //打开数据库
    if (! m_db.isValid()) {
        qDebug() << "error isValid !!!";
        return;
    }
    if (!m_db.open()) {
        qDebug() << "error open !!!";
        return;
    }

     QSqlQuery querycreate;
     querycreate.exec("CREATE TABLE IF NOT EXISTS CurrentWallpaper(path Text primary key)");
     querycreate.exec("CREATE TABLE IF NOT EXISTS WallpaperData(path Text primary key,name Text,iconpath Text,type Text)");

     //playlistPath
     querycreate.exec("CREATE TABLE IF NOT EXISTS PlaylistPath(path Text primary key)");
}

bool DBManager::execSql(const QString &str)
{
    bool bRet =false;
    QMutexLocker locker(&m_mutex);
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        bRet = query.exec(str);
    }
    return bRet;
}

QStringList DBManager::getAllPath()
{
    QStringList list;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec("select * from WallpaperData");

        while (query.next()) { //一行一行遍历
            //取出当前行的内容
            //以列为单位的     //第0列
            QString path = query.value("path").toString();
            list << path;
        }
    }
    return list;
}

QList<WallpaperData> DBManager::getAllData()
{
    QList <WallpaperData> list;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec("select * from WallpaperData");

        while (query.next()) { //一行一行遍历
            //取出当前行的内容
            //以列为单位的     //第0列
            WallpaperData data;
            data.path = query.value("path").toString();
            data.name = query.value("name").toString();
            data.iconPath = query.value("iconPath").toString();
            data.type = query.value("type").toString();
            list << data;
        }
    }
    return list;
}

int DBManager::getAllCount()
{
    int i=0;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec("select COUNT(*) from WallpaperData");
        if (query.first())
        {
            i = query.value(0).toInt();
        }
    }
    return i;
}

QString DBManager::getCurrentPath()
{
    QString str;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec("select * from CurrentWallpaper");

        while (query.next()) { //一行一行遍历
            //取出当前行的内容
            //以列为单位的     //第0列
            QString path = query.value("path").toString();
            str =path;
        }
    }
    return str;
}

bool DBManager::setCurrentPath(const QString & path)
{
    bool bRet=false;
    execSql("DELETE FROM CurrentWallpaper");
    QString strSql = QString("insert into CurrentWallpaper(path) values('%1')").arg(path);
    bRet = execSql(strSql);
    return bRet;
}

bool DBManager::isContainsPath(const QString &path)
{
    bool bRet=false;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec(QString("SELECT * FROM WallpaperData WHERE path = '%1';").arg(path));

        while (query.next()) { //一行一行遍历
            bRet =true;
        }
    }
    return bRet;
}

bool DBManager::addDatas(const QList<WallpaperData> &list)
{
    bool bRet=false;
    for(WallpaperData data : list)
    {
        if(!isContainsPath(data.path))
        {
            QString strSql = QString("insert into WallpaperData(path,name,iconpath,type) values('%1','%2','%3','%4')").arg(data.path).arg(data.name).arg(data.iconPath).arg(data.type);
            bRet = execSql(strSql);
        }
    }
    return bRet;
}

bool DBManager::clearLocalPaths()
{
    return execSql("DELETE FROM WallpaperData");
}

bool DBManager::deleteLocalPaths(const QStringList &paths)
{
    bool bRet = false;
   for(QString str :paths)
   {
       QString strSql = QString("DELETE FROM WallpaperData where path = '%1';").arg(str);
       bRet = execSql(strSql);
   }
   return bRet;
}

QStringList DBManager::getAllPlayList()
{
    QStringList list;
    if (m_db.isValid()) {
        QSqlQuery query(m_db);
        query.exec("select * from PlaylistPath");

        while (query.next()) { //一行一行遍历
            //取出当前行的内容
            //以列为单位的     //第0列
            QString path = query.value("path").toString();
            list << path;
        }
    }
    return list;
}

bool DBManager::addPlayList(const QStringList &paths)
{
     bool bRet = false;
    for(QString str :paths)
    {
        QString strSql = QString("insert into PlaylistPath(path) values('%1')").arg(str);
        bRet = execSql(strSql);
    }
    return bRet;
}

bool DBManager::clearPlayList()
{
    bool bRet = false;
    bRet = execSql("DELETE FROM PlaylistPath");
    return bRet;
}

bool DBManager::deletePlayList(const QStringList &paths)
{
    bool bRet = false;
   for(QString str :paths)
   {
       QString strSql = QString("DELETE FROM PlaylistPath where path = '%1';").arg(str);
       bRet = execSql(strSql);
   }
   return bRet;
}
