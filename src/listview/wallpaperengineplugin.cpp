#include "wallpaperengineplugin.h"
#include "ui_wallpaperengineplugin.h"

#include <QFileDialog>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>

#include "view.h"
#include "application.h"
#include "inimanager.h"

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
        QDir folder(path);
        if (folder.exists()) {
            qDebug() << "文件夹存在";
            IniManager::instance()->setValue("WallPaper/wallpaperEnginePath", dApp->m_wallpaperEnginePath);
        } else {
            qDebug() << "文件夹不存在";
        }
    }
    ui->enginePath->setText(path);
    refresh(path);
}

void wallpaperEnginePlugin::FindFile(const QString &_filePath)
{
    QDir folder(_filePath);
    QStringList files = folder.entryList(QStringList() << "*.json", QDir::Files);

    // 遍历当前文件夹下的所有json文件
    for (const QString& file : files) {
        if (file == "project.json") {
            qDebug() << folder.filePath(file);
        }
    }

    // 遍历当前文件夹下的所有子文件夹
    QStringList folders = folder.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString& subfolder : folders) {
        QString subfolderPath = folder.filePath(subfolder);
        FindFile(subfolderPath); // 递归调用，查找子文件夹中的project.json文件
    }
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

    QThread *th = QThread::create([ = ]() {
        FindFile(path);
        for (QString str : m_JasonList) {
            readJson(str);
        }
        showView();
    });
    th->start();
#else
    QFuture<void> future = QtConcurrent::run([=](){
        // 并行执行的代码
        FindFile(path);
        for (QString str : m_JasonList) {
            readJson(str);
        }
        showView();
    });
//    QFuture<void> future = QtConcurrent::run(refreashFuction,path);
#endif
}

wallpaperEnginePlugin::~wallpaperEnginePlugin()
{
    delete ui;
}

void wallpaperEnginePlugin::on_setEnginePath_clicked()
{
    QString path = QFileDialog::getExistingDirectory();
    if (!path.isEmpty() && path.toLower().contains("steam")) {
        ui->enginePath->setText(path);
    }
    else
    {
       QMessageBox::information(nullptr, tr("Error"), tr("Not containing the word steam!"));
    }
}

void wallpaperEnginePlugin::on_setBtn_clicked()
{
    QString path = ui->enginePath->text();
    if (!path.isNull() && path.toLower().contains("steam")) {
        refresh(ui->enginePath->text());
        dApp->m_wallpaperEnginePath = ui->enginePath->text();
        IniManager::instance()->setValue("WallPaper/wallpaperEnginePath", dApp->m_wallpaperEnginePath);
    }
    else
    {
       QMessageBox::information(nullptr, tr("Error"), tr("Not containing the word steam!"));
    }
}
