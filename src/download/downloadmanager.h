#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include "data.h"
class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);

signals:
    void downloadStarted(const DownloadInfo &data);
    void downloadFinished(const DownloadInfo &data);
    void downloadError(const DownloadInfo &data, const QString &errorMessage);

public slots:
    void addDownload(const DownloadInfo &data);
    void processDownloadQueue();

private:
    QList<DownloadInfo> downloadQueue;
    int activeDownloadsCount = 0;
    const int maxConcurrentDownloads = 1;
    QMutex m_mutex;
};


#endif // DOWNLOADMANAGER_H
