#ifndef INSTANCE_H
#define INSTANCE_H

#include <QString>

#ifdef Q_OS_LINUX
void load_translation_files(const QString path);
#endif

#endif // INSTANCE_H
