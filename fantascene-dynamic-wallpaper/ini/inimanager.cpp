#include "inimanager.h"
#include <QDir>
const QString CONFIG_PATH =   QDir::homePath() +
        "/.config/fantascene-dynamic-wallpaper/config.ini";

IniManager::IniManager(QObject *parent)
    :QObject(parent)
{
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
