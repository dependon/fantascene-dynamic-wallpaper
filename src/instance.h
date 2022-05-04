#ifndef INSTANCE_H
#define INSTANCE_H

#include <QString>
#include <QDir>
#include <QDirIterator>
#include <QLockFile>
#include <QDebug>
#include <QApplication>
#include <QTranslator>

bool check_instance_status(const QString path, const QString file);
#ifdef Q_OS_LINUX
void load_translation_files(const QString path);
#endif

#endif // INSTANCE_H
