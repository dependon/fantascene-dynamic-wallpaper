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
