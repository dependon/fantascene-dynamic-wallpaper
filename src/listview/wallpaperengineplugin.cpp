#include "wallpaperengineplugin.h"
#include "ui_wallpaperengineplugin.h"

#include <QFileDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>

#include "view.h"
#include "application.h"

wallpaperEnginePlugin::wallpaperEnginePlugin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wallpaperEnginePlugin)
{
    ui->setupUi(this);
    m_view = new view();
    ui->horizontalLayout->addWidget(m_view);
    setWindowTitle("wallpaper engine plugin");

    QString path = QDir::homePath() +
                   "/.local/share/Steam/steamapps/workshop/content";
    if (!dApp->m_wallpaperEnginePath.isNull()) {
        path = dApp->m_wallpaperEnginePath;
    } else {
        dApp->m_wallpaperEnginePath = path;
    }
    ui->enginePath->setText(path);
    refresh(path);
}

void wallpaperEnginePlugin::FindFile(const QString &_filePath)
{
    QDir dir(_filePath);
    if (!dir.exists()) {
        return ;
    }

    //取到所有的文件和文件名，但是去掉.和..的文件夹（这是QT默认有的）
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    //文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    if (list.size() < 1) {
        return ;
    }
    int i = 0;

    //递归算法的核心部分
    do {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool bisDir = fileInfo.isDir();
        if (bisDir) {
            FindFile(fileInfo.filePath());
        } else {
            //bool isDll = fileInfo.fileName().endsWith(".dll");
            qDebug() << fileInfo.filePath() << ":" << fileInfo.fileName();
            if (fileInfo.fileName().contains("project.json")) {
                m_JasonList.push_back(fileInfo.filePath());
            };
        }//end else
        i++;
    } while (i < list.size());
    return;
}

void wallpaperEnginePlugin::readJson(const QString &path)
{
    QFile loadFile(path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "could't open projects json";
        return;
    }
    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(allData);

    QJsonObject rootObj = jsonDoc.object();

    QStringList keys = rootObj.keys();
    wallpaperEengineData data;
    data.JsonPath = path;
    data.JsonBasePath = QFileInfo(path).path();

    data.descrption = rootObj.value("descrption").toString();
    data.path = rootObj.value("path").toString();
    data.general = rootObj.value("general").toString();
    data.preview = rootObj.value("preview").toString();
    data.tags = rootObj.value("tags").toString();
    data.title = rootObj.value("title").toString();
    data.type = rootObj.value("type").toString();
    data.visibility = rootObj.value("visibility").toString();
    data.file = rootObj.value("file").toString();
    m_map.insert(data.JsonPath, data);
}

void wallpaperEnginePlugin::showView()
{
    QStringList list ;
    for (wallpaperEengineData data : m_map) {

        QString videoPath = data.JsonBasePath + "/" + data.file;
        if (QFileInfo(videoPath).isFile()) {
//            if (!QFileInfo(videoPath).suffix().contains("html")) {
                list.push_back(videoPath);
//            };
        }

    }
    m_view->setFiles(list);
    if (this->size() == QSize(800, 600)) {
        m_view->resize(QSize(801, 601));
    } else {
        m_view->resize(QSize(800, 600));
    }

}

void wallpaperEnginePlugin::refresh(const QString &path)
{
    QThread *th = QThread::create([ = ]() {
        FindFile(path);
        for (QString str : m_JasonList) {
            readJson(str);
        }
        showView();
    });
    th->start();

}

wallpaperEnginePlugin::~wallpaperEnginePlugin()
{
    delete ui;
}

void wallpaperEnginePlugin::on_setEnginePath_clicked()
{
    QString path = QFileDialog::getExistingDirectory();
    if (!path.isEmpty() && path.contains("Steam")) {
        ui->enginePath->setText(path);
    }
}

void wallpaperEnginePlugin::on_setBtn_clicked()
{
    QString path = ui->enginePath->text();
    if (!path.isNull() && path.contains("Steam")) {
        refresh(ui->enginePath->text());
        dApp->m_wallpaperEnginePath = ui->enginePath->text();
    }
}
