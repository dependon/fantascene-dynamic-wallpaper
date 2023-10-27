#ifndef DBMANAGER1_H
#define DBMANAGER1_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QMutex>
#include <QString>
#include <QVariant>
#include <QDebug>
struct WallpaperData
{
    QString path;
    QString name;
    QString iconPath;
    QString type;
};
class DBManager : public QObject
{
    Q_OBJECT
public:
    static DBManager* instance();

    bool execSql(const QString &str);

    QStringList getAllPath();
    QList <WallpaperData> getAllData();
    int getAllCount();
    QString getCurrentPath();
    bool setCurrentPath(const QString &path);
    bool isContainsPath(const QString& path);
    bool addDatas(const QList <WallpaperData> & list);
    bool clearLocalPaths();
    bool deleteLocalPaths(const QStringList &paths);

    //playlistPath
    QStringList getAllPlayList();
    bool addPlayList(const QStringList & paths);
    bool clearPlayList();
    bool deletePlayList(const QStringList &paths);
Q_SIGNALS:

private Q_SLOTS:

private:
    explicit DBManager(QObject *parent = nullptr);
    static DBManager * dbmanager;
    QSqlDatabase m_db;
    QMutex m_mutex;
};

#endif // DBMANAGER1_H
