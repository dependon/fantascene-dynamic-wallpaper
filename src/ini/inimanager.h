#ifndef INIMANAGER_H
#define INIMANAGER_H

#include <QObject>
#include <QSettings>
class IniManager: public QObject
{
    Q_OBJECT
public:
    // 单实例
    static IniManager *instance();

    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    void remove(const QString &key);
    bool contains(const QString &key) const;
    void clear();

private:
    IniManager(QObject *parent=nullptr);
    QSettings* m_settings{nullptr};
};

#endif // INIMANAGER_H
