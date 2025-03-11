#ifndef OSSTATUS_H
#define OSSTATUS_H


#include <fstream>
#include <sstream>
#include <iostream>

#include <QObject>

#ifdef Q_OS_WIN
#include <pdh.h>
#include <pdhmsg.h>
#include <windows.h>
#pragma comment(lib, "pdh.lib")
#else
#include <dirent.h>
#endif
// 获取内存使用信息的结构体
struct MemoryInfo {
    long long used;
    long long total;
};

// 计算CPU使用率的函数
double getCpuUsage() {
    static long long prevTotal = 0;
    static long long prevIdle = 0;

    std::ifstream statFile("/proc/stat");
    if (!statFile.is_open()) {
        return -1;
    }

    std::string line;
    std::getline(statFile, line);
    statFile.close();

    std::istringstream iss(line);
    std::string cpuLabel;
    iss >> cpuLabel;

    long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    long long idleDiff = idle - prevIdle;
    long long totalDiff = total - prevTotal;

    prevTotal = total;
    prevIdle = idle;

    if (totalDiff == 0) {
        return 0;
    }

    return (1.0 - static_cast<double>(idleDiff) / totalDiff) * 100.0;
}

#ifdef Q_OS_WIN
// 获取 Windows 系统的内存使用率
double getMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo)) {
        return -1;
    }

    DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
    DWORDLONG usedPhysMem = totalPhysMem - memInfo.ullAvailPhys;
    if (totalPhysMem == 0) {
        return 0;
    }
    return static_cast<double>(usedPhysMem) / totalPhysMem * 100;
}
#elif defined(Q_OS_LINUX)
// 获取 Linux 系统的内存使用率
double getMemoryUsage() {
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        return -1;
    }

    long long totalMemory = 0, freeMemory = 0, buffers = 0, cached = 0;
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

    long long usedMemory = totalMemory - freeMemory - buffers - cached;
    if (totalMemory == 0) {
        return 0;
    }
    return static_cast<double>(usedMemory) / totalMemory * 100;
}
#endif


#ifdef Q_OS_WIN
// 获取 Windows 系统的内存使用信息
MemoryInfo getWindowsMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&memInfo)) {
        return {-1, -1};
    }

    long long totalPhysMem = static_cast<long long>(memInfo.ullTotalPhys);
    long long usedPhysMem = totalPhysMem - static_cast<long long>(memInfo.ullAvailPhys);
    return {usedPhysMem, totalPhysMem};
}
#elif defined(Q_OS_LINUX)
// 获取 Linux 系统的内存使用信息
MemoryInfo getLinuxMemoryInfo() {
    std::ifstream meminfo("/proc/meminfo");
    if (!meminfo.is_open()) {
        return {-1, -1};
    }

    long long totalMemory = 0, freeMemory = 0, buffers = 0, cached = 0;
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

    long long usedMemory = totalMemory - freeMemory - buffers - cached;
    return {usedMemory, totalMemory};
}
#endif
// 根据不同系统调用对应的内存信息获取函数
MemoryInfo getApiMemoryInfo() {
#ifdef Q_OS_WIN
    return getWindowsMemoryInfo();
#elif defined(Q_OS_LINUX)
    return getLinuxMemoryInfo();
#else
    return {-1, -1};
#endif
}

#ifdef Q_OS_WIN
// 性能计数器句柄
PDH_HQUERY query;
PDH_HCOUNTER rxCounter;
PDH_HCOUNTER txCounter;


// 初始化性能计数器
bool initNetworkCounters() {
    if (PdhOpenQuery(nullptr, NULL, &query) != ERROR_SUCCESS) {
        return false;
    }
    if (PdhAddCounter(query, L"\\Network Interface(*)\\Bytes Received/sec", NULL, &rxCounter) != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        return false;
    }
    if (PdhAddCounter(query, L"\\Network Interface(*)\\Bytes Sent/sec", NULL, &txCounter) != ERROR_SUCCESS) {
        PdhRemoveCounter(rxCounter);
        PdhCloseQuery(query);
        return false;
    }
    return true;
}

// 获取网络速度
std::pair<double, double> getNetworkSpeed() {
    PDH_FMT_COUNTERVALUE rxValue;
    PDH_FMT_COUNTERVALUE txValue;
    if (PdhCollectQueryData(query) != ERROR_SUCCESS) {
        return {-1, -1};
    }
    if (PdhGetFormattedCounterValue(rxCounter, PDH_FMT_DOUBLE, nullptr, &rxValue) != ERROR_SUCCESS) {
        return {-1, -1};
    }
    if (PdhGetFormattedCounterValue(txCounter, PDH_FMT_DOUBLE, nullptr, &txValue) != ERROR_SUCCESS) {
        return {-1, -1};
    }
    double rxSpeed = rxValue.doubleValue / 1024.0;  // 转换为 KB/s
    double txSpeed = txValue.doubleValue / 1024.0;
    return {rxSpeed, txSpeed};
}
#elif defined(Q_OS_LINUX)
// 读取指定网络接口的接收和发送字节数
std::pair<long long, long long> getNetworkBytes(const std::string& interface) {
    std::ifstream rxFile("/sys/class/net/" + interface + "/statistics/rx_bytes");
    std::ifstream txFile("/sys/class/net/" + interface + "/statistics/tx_bytes");
    long long rxBytes = 0, txBytes = 0;
    if (rxFile.is_open()) {
        rxFile >> rxBytes;
        rxFile.close();
    }
    if (txFile.is_open()) {
        txFile >> txBytes;
        txFile.close();
    }
    return {rxBytes, txBytes};
}

// 存储上次记录的每个接口的接收和发送字节数
std::unordered_map<std::string, std::pair<long long, long long>> prevBytes;

// 获取所有网络接口的速度
std::unordered_map<std::string, std::pair<double, double>> getAllNetworkSpeeds() {
    std::unordered_map<std::string, std::pair<double, double>> speeds;
    DIR* dir = opendir("/sys/class/net");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string interface = entry->d_name;
            if (interface != "." && interface != "..") {
                auto [rxBytes, txBytes] = getNetworkBytes(interface);
                if (prevBytes.find(interface) != prevBytes.end()) {
                    double rxSpeed = (rxBytes - prevBytes[interface].first) / 1024.0;  // 转换为 KB/s
                    double txSpeed = (txBytes - prevBytes[interface].second) / 1024.0;
                    speeds[interface] = {rxSpeed, txSpeed};
                }
                prevBytes[interface] = {rxBytes, txBytes};
            }
        }
        closedir(dir);
    }
    return speeds;
}
#endif

#endif // OSSTATUS_H
