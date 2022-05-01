#ifndef INSTANCE_H
#define INSTANCE_H

#include <QString>
#include <QDir>
#include <QLockFile>
#include <QDebug>

bool check_instance_status(const QString path, const QString file);

#endif // INSTANCE_H
