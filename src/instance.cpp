/*
 * Check instance lock path
 *
 * @ Accepts parameters are : path (const), lock (const)
 *
 * @ Returns true if status checks are successful
 */

#include "instance.h"

#include <QDir>
#include <QDirIterator>
#include <QLockFile>
#include <QDebug>
#include <QApplication>
#include <QTranslator>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

bool check_instance_status(const QString pathFile, const QString file)
{
    //single
    QString userName = QDir::homePath().section("/", -1, -1);
    std::string path = (QDir::homePath() +pathFile).toStdString();
    QDir tdir(path.c_str());
    if (!tdir.exists()) {
        bool ret =  tdir.mkpath(path.c_str());
        qDebug() << ret ;
    }

    path += "single";
    int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0644);
    int flock = lockf(fd, F_TLOCK, 0);

    if (fd == -1) {
        perror("open lockfile/n");
        return false;
    }
    if (flock == -1) {
        perror("lock file error/n");
        return false;
    }
    return true;
}

#ifdef Q_OS_LINUX
/*
 * Load translation files
 *
 * @ Accepts parameters are : path (const)
 *
 * @ Returns no results
 */
void load_translation_files(const QString path)
{
    QDir dir(path);
    if (dir.exists())
    {
        QDirIterator qmIt(path, QStringList() << QString("*%1.qm").arg(QLocale::system().name()), QDir::Files);
        while (qmIt.hasNext())
        {
            qmIt.next();
            QFileInfo finfo = qmIt.fileInfo();
            QString tr_path = finfo.absolutePath();
            QString tr_file = finfo.baseName();
            QTranslator *translator = new QTranslator;

            if (!translator->load(tr_file,tr_path)) {
                qWarning() << "Failed to load " + tr_path, + "/" + tr_file + " ,fallback to default!";
            }
            if (!qApp->installTranslator(translator)) {
                qWarning() << "Failed to install translation!";
            }
        }
    }
}
#endif
