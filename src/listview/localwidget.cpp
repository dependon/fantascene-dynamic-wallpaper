#include "localwidget.h"
#include "ui_localwidget.h"
#include "view.h"
#include <QStandardPaths>
#include <QDirIterator>
#include <QFileSystemWatcher>
#include <QDebug>
#include <QDir>
const QStringList filetypes {
    "3g2", "3ga", "3gp", "3gp2", "3gpp", "amv", "asf", "asx", "avf", "avi", "bdm", "bdmv",\
    "bik", "clpi", "cpi", "dat", "divx", "drc", "dv", "dvr-ms", "f4v", "flv", "gvi", "gxf", \
    "hdmov", "hlv", "iso", "letv", "lrv", "m1v", "m2p", "m2t", "m2ts", "m2v", "m3u", "m3u8",\
    "m4v", "mkv", "moov", "mov", "mov", "mp2", "mp2v", "mp4", "mp4v", "mpe", "mpeg", "mpeg1",\
    "mpeg2", "mpeg4", "mpg", "mpl", "mpls", "mpv", "mpv2", "mqv", "mts", "mts", "mtv", "mxf", \
    "mxg", "nsv", "nuv", "ogg", "ogm", "ogv", "ogx", "ps", "qt", "qtvr", "ram", "rec", "rm", \
    "rm", "rmj", "rmm", "rms", "rmvb", "rmx", "rp", "rpl", "rv", "rvx", "thp", "tod", "tp",\
    "trp", "ts", "tts", "txd", "vcd", "vdr", "vob", "vp8", "vro", "webm", "wm", "wmv", "wtv", "xesc", "xspf",\
    "mp3", "ogg", "wav", "wma", "m4a", "aac", "ac3", "ape", "flac", "ra", "mka", "dts", "opus","html","htm"
};

LocalWidget::LocalWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LocalWidget)
{
    ui->setupUi(this);
    ui->playBtn->hide();
    m_viewLocal = new view(this);
    ui->verticalLayout->addWidget(m_viewLocal);
#ifdef Q_OS_WIN
    m_strLocalPATH = QApplication::applicationDirPath()+"/fantascene";
#else
    m_strLocalPATH = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene";
#endif
    setWindowTitle(tr("Wallpaper Local"));
    searchVideoFiles(m_strLocalPATH);
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
void LocalWidget::searchVideoFiles(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
        return ;
    }
    // 遍历当前目录下的所有文件和文件夹
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& fileInfo: fileList) {
        if (fileInfo.isDir()) {
            // 如果是文件夹，则递归调用函数
            searchVideoFiles(fileInfo.absoluteFilePath());
        } else {
            // 如果是文件，则检查文件是否为视频文件
            QString extension = fileInfo.suffix().toLower();
             if (filetypes.contains(extension)) {
                // 这是一个视频文件，进行相应的操作
                m_allPath << fileInfo.absoluteFilePath();
            }
        }
    }
}

void LocalWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void LocalWidget::on_playBtn_clicked()
{

}

void LocalWidget::on_directoryChanged(const QString &path)
{
    qDebug()<< path;
    m_allPath.clear();
    searchVideoFiles(m_strLocalPATH);
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
