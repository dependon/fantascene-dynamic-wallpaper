#include "localwidget.h"
#include "ui_localwidget.h"
#include "view.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>
LocalWidget::LocalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalWidget)
{
    ui->setupUi(this);
    ui->playBtn->hide();
    m_viewLocal = new view(this);
    ui->verticalLayout->addWidget(m_viewLocal);
    m_strLocalPATH = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene/";

    setWindowTitle(tr("Wallpaper Local"));
    m_allPath = getAllpath();
    m_viewLocal->setFiles(m_allPath);
    m_fileWacher =new QFileSystemWatcher(this);
    m_fileWacher->addPath(m_strLocalPATH);
    QObject::connect(m_fileWacher, &QFileSystemWatcher::directoryChanged,this, &LocalWidget::on_directoryChanged);
    watchDirectory(m_strLocalPATH,*m_fileWacher);
//    QObject::connect(m_fileWacher, &QFileSystemWatcher::fileChanged, this,&LocalWidget::on_fileChanged);

    QString str=tr("Please place the local video on:");
    ui->label->setText(str+m_strLocalPATH);
}

LocalWidget::~LocalWidget()
{
    delete ui;
}

QStringList LocalWidget::getAllpath()
{
    QDir dir(m_strLocalPATH);

    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QStringList reStrList;
    QDirIterator it(m_strLocalPATH, QDir::Files |  QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString filePath = it.next();

        reStrList << filePath;
    }
    return reStrList;
}

void LocalWidget::on_playBtn_clicked()
{

}

void LocalWidget::on_directoryChanged(const QString &path)
{
    qDebug()<< path;
    m_allPath = getAllpath();
    m_viewLocal->setFiles(m_allPath);
    m_viewLocal->resize(m_viewLocal->width()+1,m_viewLocal->height());
    m_viewLocal->resize(m_viewLocal->width()-1,m_viewLocal->height());

     watchDirectory(m_strLocalPATH,*m_fileWacher);
}

void LocalWidget::watchDirectory(const QString& path, QFileSystemWatcher& watcher)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    watcher.addPath(path);
    qDebug() << "Watching directory:" << path;

    QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& subdir : subdirs) {
        QString subPath = path + QDir::separator() + subdir;
        watchDirectory(subPath, watcher);
    }
}

void LocalWidget::on_fileChanged(const QString &path)
{
    //未知原因,不触发
    return ;
    if(QFile::exists(path))
    {
        if(!m_allPath.contains(path))
        {
            m_allPath.push_back(path);
            m_viewLocal->addPath(path);

        }

    }
    else {
        if(m_allPath.contains(path))
        {
            m_allPath.push_back(path);
            m_viewLocal->removePath(path);
        }
    }
}
