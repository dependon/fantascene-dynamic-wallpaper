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
void load_translation_files(const QString path);

#endif // INSTANCE_H
