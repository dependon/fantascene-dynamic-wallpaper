#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class SystemMonitor : public QObject
{
    Q_OBJECT
public:
    static SystemMonitor* getInstance();
    explicit SystemMonitor(QObject *parent = nullptr);

    //Cpu
    double getCpuUsage();
    QString getCpuModel();
    QString cpuModel;
    QVector<double> cpuUsageData;

    //Memory
    double getMemoryUsage();
    QString getMemoryInfo();
    QString memoryInfo;
    QVector<double> memoryUsageData;

    //NETWORK
    double getUploadSpeed();
    double getDownloadSpeed();
    void updateNetworkSpeed();
    QVector<double> uploadSpeedData;
    QVector<double> downloadSpeedData;


    bool m_readCpu = true;
    bool m_readMemory = true;
    bool m_readNetwork = true;

Q_SIGNALS:
    void cpuUsageChanged(const QVector<double>& usage);
    void memoryUsageChanged(const QVector<double>& usage);

    void uploadSpeedChanged(const QVector<double>& speed);
    void downloadSpeedChanged(const QVector<double>& speed);

private Q_SLOTS:
    void updateUsage();

private:
    QTimer *timer;

    static SystemMonitor* instance;

    double uploadSpeed;
    double downloadSpeed;
    double lastBytesSent;
    double lastBytesReceived;


};

#endif // SYSTEMMONITOR_H
