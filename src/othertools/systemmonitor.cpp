#include "systemmonitor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "application.h"
#include "inimanager.h"
#include <QRegularExpression>
SystemMonitor* SystemMonitor::instance = nullptr;

SystemMonitor* SystemMonitor::getInstance()
{
    if (instance == nullptr) {
        instance = new SystemMonitor();
    }
    return instance;
}

SystemMonitor::SystemMonitor(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SystemMonitor::updateUsage);
    timer->start(1000); // 每秒更新一次

    cpuUsageData.resize(61);
    memoryUsageData.resize(61);

    cpuModel = getCpuModel();
    memoryInfo = getMemoryInfo();
}

// 删除多余的析构函数
// SystemMonitor::~SystemMonitor()
// {
//     // 这里可以添加析构时需要执行的代码，如果原代码有相关逻辑可补充进来
// }

double SystemMonitor::getCpuUsage()
{
#ifdef Q_OS_WIN
    // Windows系统
    static FILETIME prevIdleTime, prevKernelTime, prevUserTime;
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER prevIdle, prevKernel, prevUser;
        ULARGE_INTEGER idle, kernel, user;

        prevIdle.LowPart = prevIdleTime.dwLowDateTime;
        prevIdle.HighPart = prevIdleTime.dwHighDateTime;
        prevKernel.LowPart = prevKernelTime.dwLowDateTime;
        prevKernel.HighPart = prevKernelTime.dwHighDateTime;
        prevUser.LowPart = prevUserTime.dwLowDateTime;
        prevUser.HighPart = prevUserTime.dwHighDateTime;

        idle.LowPart = idleTime.dwLowDateTime;
        idle.HighPart = idleTime.dwHighDateTime;
        kernel.LowPart = kernelTime.dwLowDateTime;
        kernel.HighPart = kernelTime.dwHighDateTime;
        user.LowPart = userTime.dwLowDateTime;
        user.HighPart = userTime.dwHighDateTime;

        ULONGLONG idleDiff = idle.QuadPart - prevIdle.QuadPart;
        ULONGLONG kernelDiff = kernel.QuadPart - prevKernel.QuadPart;
        ULONGLONG userDiff = user.QuadPart - prevUser.QuadPart;
        ULONGLONG totalDiff = kernelDiff + userDiff;

        prevIdleTime = idleTime;
        prevKernelTime = kernelTime;
        prevUserTime = userTime;

        if (totalDiff > 0) {
            return (1.0 - (double)idleDiff / totalDiff) * 100.0;
        }
    }
    return 0.0;
#else
    // Linux系统
    QFile file("/proc/stat");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList parts = line.split(' ');
        if (parts.size() >= 5) {
            static qint64 prevIdle = 0;
            static qint64 prevTotal = 0;

            qint64 user = parts[2].toLongLong();
            qint64 nice = parts[3].toLongLong();
            qint64 system = parts[4].toLongLong();
            qint64 idle = parts[5].toLongLong();
            qint64 total = user + nice + system + idle;

            qint64 idleDiff = idle - prevIdle;
            qint64 totalDiff = total - prevTotal;

            prevIdle = idle;
            prevTotal = total;

            if (totalDiff > 0) {
                return (1.0 - (double)idleDiff / totalDiff) * 100.0;
            }
        }
        file.close();
    }
    return 0.0;
#endif
}

double SystemMonitor::getMemoryUsage()
{
#ifdef Q_OS_WIN
    // Windows系统
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (1.0 - (double)memInfo.ullAvailPhys / memInfo.ullTotalPhys) * 100.0;
#else
    // Linux系统
    QFile file("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        qint64 totalMemory = 0;
        qint64 freeMemory = 0;
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("MemTotal:")) {
                QStringList parts = line.split(' ');
                for (const QString &part : parts) {
                    if (!part.isEmpty()) {
                        totalMemory = part.toLongLong();
                        break;
                    }
                }
            } else if (line.startsWith("MemFree:")) {
                QStringList parts = line.split(' ');
                for (const QString &part : parts) {
                    if (!part.isEmpty()) {
                        freeMemory = part.toLongLong();
                        break;
                    }
                }
            }
        }
        file.close();
        if (totalMemory > 0) {
            return (1.0 - (double)freeMemory / totalMemory) * 100.0;
        }
    }
    return 0.0;
#endif
}

QString SystemMonitor::getCpuModel()
{
#ifdef Q_OS_WIN
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    char cpuBrand[49];
    __cpuid((int*)cpuBrand, 0x80000002);
    __cpuid((int*)&cpuBrand[16], 0x80000003);
    __cpuid((int*)&cpuBrand[32], 0x80000004);
    return QString::fromUtf8(cpuBrand);
#else
    QFile file("/proc/cpuinfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("model name")) {
                QStringList parts = line.split(':');
                if (parts.size() >= 2) {
                    return parts[1].trimmed();
                }
            }
        }
        file.close();
    }
    return "Unknown";
#endif
}

QString SystemMonitor::getMemoryInfo()
{
#ifdef Q_OS_WIN
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&memInfo);
        double totalMemory = static_cast<double>(memInfo.ullTotalPhys) / (1024 * 1024 * 1024);
        double usedMemory = static_cast<double>(memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024 * 1024);
        return QString("%1GB/%2GB").arg(usedMemory, 0, 'f', 1).arg(totalMemory, 0, 'f', 1);
#else
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        return "0/0";
    }

    double totalMemory = 0, freeMemory = 0, buffers = 0, cached = 0;
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> totalMemory;
        } else if (line.find("MemFree:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> freeMemory;
        } else if (line.find("Buffers:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> buffers;
        } else if (line.find("Cached:") != std::string::npos) {
            std::istringstream iss(line);
            std::string label;
            iss >> label >> cached;
        }
    }
    meminfo.close();

    double usedMemory = totalMemory - freeMemory - buffers - cached;


    if (totalMemory == 0) {
        return 0;
    }
    else
    {
        double memoryUsage = ((double)usedMemory / totalMemory) * 100.0;
        // 更新数据
        memoryUsageData.removeFirst();
        memoryUsageData.append(memoryUsage);

        totalMemory = (double)totalMemory / (1024.0 * 1024.0);
        usedMemory = (double)usedMemory / (1024.0 * 1024.0);
    }
    return QString("%1GB/%2GB").arg(usedMemory, 0, 'f', 1).arg(totalMemory, 0, 'f', 1);
#endif
}

double SystemMonitor::getUploadSpeed()
{
    return uploadSpeedData.last();
}

double SystemMonitor::getDownloadSpeed()
{
    return downloadSpeedData.last();
}

void SystemMonitor::updateNetworkSpeed()
{
#ifdef Q_OS_WIN
    PMIB_IF_TABLE2 pIfTable = nullptr;
    if (GetIfTable2(&pIfTable) == NO_ERROR) {
        qint64 newSent = 0;
        qint64 newReceived = 0;

        for (ULONG i = 0; i < pIfTable->NumEntries; i++) {
            MIB_IF_ROW2& row = pIfTable->Table[i];
            if (row.Type != IF_TYPE_SOFTWARE_LOOPBACK) {
                newSent += row.OutOctets;
                newReceived += row.InOctets;
            }
        }

        uploadSpeed = (newSent - lastBytesSent) / 1024.0; // KB/s
        downloadSpeed = (newReceived - lastBytesReceived) / 1024.0;

        lastBytesSent = newSent;
        lastBytesReceived = newReceived;

        FreeMibTable(pIfTable);
    }
#else
    QFile file("/proc/net/dev");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.readLine(); // 跳过前两行标题
        in.readLine();

        qint64 newSent = 0;
        qint64 newReceived = 0;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            QStringList parts = line.split(QRegularExpression("\\s+"));
            if (parts.size() > 9) {
                newReceived += parts[1].toLongLong();
                newSent += parts[9].toLongLong();
            }
        }

        uploadSpeed = (newSent - lastBytesSent) / 1024.0;
        downloadSpeed = (newReceived - lastBytesReceived) / 1024.0;

        lastBytesSent = newSent;
        lastBytesReceived = newReceived;
    }
#endif

    uploadSpeedData.append(uploadSpeed);
    downloadSpeedData.append(downloadSpeed);

    if (uploadSpeedData.size() > 61) {
        uploadSpeedData.removeFirst();
    }
    if (downloadSpeedData.size() > 61) {
        downloadSpeedData.removeFirst();
    }

    Q_EMIT uploadSpeedChanged(uploadSpeedData);
    Q_EMIT downloadSpeedChanged(downloadSpeedData);
}

void SystemMonitor::updateUsage()
{
    if(m_readCpu)
    {
        double cpuUsage = getCpuUsage();
        // 更新数据
        cpuUsageData.removeFirst();
        cpuUsageData.append(cpuUsage);

        Q_EMIT cpuUsageChanged(cpuUsageData);
    }
    if(m_readMemory)
    {
        memoryInfo= getMemoryInfo();
        Q_EMIT memoryUsageChanged(memoryUsageData);
    }
    if(m_readNetwork)
    {
        // 更新网络速度
        updateNetworkSpeed();
        double uploadSpeed = getUploadSpeed();
        double downloadSpeed = getDownloadSpeed();
    }
}
