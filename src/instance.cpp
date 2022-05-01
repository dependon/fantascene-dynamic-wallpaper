/*
 * Check instance lock path
 *
 * @ Accepts parameters are : path (const), lock (const)
 *
 * @ Returns true if status checks are successful
 */

#include "instance.h"

bool check_instance_status(const QString path, const QString file)
{
    const QString lock = QDir::homePath() + "/" + path + file;
    QLockFile lockFile(lock);

    if (!lockFile.tryLock(300))
    {
        qDebug() << "The application is already running!";
        return false;
    }
    return true;
}
