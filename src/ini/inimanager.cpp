#include "inimanager.h"
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "application.h"
const QString CONFIG_PATH =   QDir::homePath() +
        "/.config/fantascene-dynamic-wallpaper/config.ini";

QString getOsVersionId() {
    QFile file("/etc/os-release");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("VERSION_ID=")) {
            file.close();
            return line.mid(11, line.length() - 12);  // 从 VERSION_ID=" 开始截取到倒数第二个字符
        }
    }

    file.close();
    return QString();
}

QString getOsName() {
    QFile file("/etc/os-release");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("NAME=")) {
            file.close();
            return line.mid(5, line.length() - 6);  // 从 NAME=" 开始截取到倒数第二个字符
        }
    }

    file.close();
    return QString();
}


IniManager::IniManager(QObject *parent)
    :QObject(parent)
{
    QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
    QString strOsName = getOsName();
    qDebug()<< strOsName;
    //QFile file(CONFIG_PATH);
    if (desktop.contains("GNOME", Qt::CaseInsensitive))
    {
        dApp->m_moreData.isShowDesktopIcon = false;
        dApp->m_moreData.isTop = false;
    }
    else if (strOsName.contains("GXDE", Qt::CaseInsensitive))
    {
        dApp->m_isGXDE = true;
        dApp->m_moreData.isShowDesktopIcon = false;
        dApp->m_moreData.isTop = false;
    }
    else if(desktop.contains("DDE", Qt::CaseInsensitive))
    {
        QString versionId = getOsVersionId();
        versionId.remove("\"");
        QStringList versionParts = versionId.split(".");
        if(versionParts.size() > 0)
        {
            double version = versionParts.at(0).toDouble();
            if (version >= 23) {
                dApp->m_isDDE23 = true;
                dApp->m_moreData.isShowDesktopIcon = false;
                dApp->m_moreData.isTop = false;
            }
        }
    }
    else if(desktop.contains("UKUI", Qt::CaseInsensitive))
    {
        dApp->m_isUKUI = true;
        dApp->m_moreData.isShowDesktopIcon = false;
        dApp->m_moreData.isTop = false;
    }
    else if(desktop.contains("Lingmo", Qt::CaseInsensitive))
    {
        dApp->m_isLingMo = true;
        dApp->m_moreData.isShowDesktopIcon = false;
        dApp->m_moreData.isTop = false;
    }

    m_settings =new QSettings(CONFIG_PATH, QSettings::IniFormat);
}

IniManager *IniManager::instance()
{
    static IniManager *pInstance = nullptr;
    if (nullptr == pInstance)
    {
        pInstance = new IniManager();
    }
    return pInstance;
}

void IniManager::setValue(const QString &key, const QVariant &value)
{
    m_settings->setValue(key,value);
}

QVariant IniManager::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings->value(key,defaultValue);
}

void IniManager::remove(const QString &key)
{
    return m_settings->remove(key);
}

bool IniManager::contains(const QString &key) const
{
    return m_settings->contains(key);
}

void IniManager::clear()
{
    m_settings->clear();
}
