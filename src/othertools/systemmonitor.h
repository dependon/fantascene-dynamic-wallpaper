#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#ifdef Q_OS_WIN
#include "windows.h"
#endif
class SystemMonitor : public QObject
{
    Q_OBJECT
public:
    static SystemMonitor* getInstance();
    explicit SystemMonitor(QObject *parent = nullptr);

    double getCpuUsage();
    double getMemoryUsage();

    QString getCpuModel();
    QString getMemoryInfo();
    
    QString cpuModel;
    QString memoryInfo;

    QVector<double> cpuUsageData;
    QVector<double> memoryUsageData;

Q_SIGNALS:
    void cpuUsageChanged(const QVector<double>& usage);
    void memoryUsageChanged(const QVector<double>& usage);

private Q_SLOTS:
    void updateUsage();

private:
    QTimer *timer;

    static SystemMonitor* instance;

};

#endif // SYSTEMMONITOR_H
