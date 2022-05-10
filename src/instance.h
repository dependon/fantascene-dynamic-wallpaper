#ifndef INSTANCE_H
#define INSTANCE_H

#include <QString>


bool check_instance_status(const QString pathFile, const QString file);
#ifdef Q_OS_LINUX
void load_translation_files(const QString path);
#endif

#endif // INSTANCE_H
