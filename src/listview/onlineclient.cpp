#include "onlineclient.h"
#include "ui_onlineclient.h"
#include "view.h"
#include "tcpclient.h"
#include <QProcess>
#include <QStandardPaths>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>
#include "application.h"

bool OnlineClient::downloadFileWithCurl(const QString &url, const QString &outputFilePath) {
    // 构建curl命令
    QString command = "curl -o " + outputFilePath + " " + url;

    // 创建QProcess对象
    QProcess process;

    // 启动进程并执行命令
    process.start(command);
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process: " + process.errorString();
        return false;
    }

    // 等待命令执行完成
    process.waitForFinished();

    // 检查命令执行结果
    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        qDebug() << "Download finished successfully.";
    } else {
        qDebug() << "Download failed with exit code: " + QString::number(process.exitCode());
        return false;
    }
    return true;
}

OnlineClient::OnlineClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineClient)
{
    ui->setupUi(this);
    m_viewDowload = new view(this);
    ui->verticalLayout->addWidget(m_viewDowload);

    m_client = new TcpClient("192.168.40.48",1234);
    m_client->run();

    QString path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene";

    if (!QDir(path).exists()) {
        // 路径不存在，尝试创建路径
        QDir dir;
        if (dir.mkpath(path)) {
            qDebug() << "Path created:" << path;
        }
    }
    connect(m_client,&TcpClient::sigShowData,this,&OnlineClient::slotShowData);
    connect(m_viewDowload,&view::sigClickedChange,this,&OnlineClient::slotClickedChange);

    connect(m_viewDowload,&view::sigDoubleClicked,this,&OnlineClient::slotDoubleClickedChange);
}

OnlineClient::~OnlineClient()
{
    delete ui;
}

void OnlineClient::on_btn_download_clicked()
{
    QString name = m_datas.value(m_currentMd5).name;
    QString newName = name.replace(QRegExp("\\s+"), "");
    QString saveFile = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene/"+newName;
    bool isExists = QFileInfo(saveFile).exists();
    if(!isExists)
    {
        QFuture<void> future = QtConcurrent::run([=](){
            bool bDown = downloadFileWithCurl(m_datas.value(m_currentMd5).downloadPath,
                                 saveFile);
            if(bDown && QFileInfo(saveFile).exists())
            {
                ui->label_Count->setText(QString::number(ui->label_Count->text().toInt()+1));
                m_client->sendData(u8"VIDEO_COUNT_ADD|"+m_currentMd5.toLatin1());
            }
            else
            {
                QMessageBox::information(nullptr, tr("Error"), tr("Dowlaod Error!"));
            }
        });
    }
    else
    {
        QMessageBox::information(nullptr, tr("Error"), tr("File Exists!"));
    }
}

void OnlineClient::slotClickedChange(const QString &md5)
{
    m_currentMd5 = md5;
    if(m_datas.keys().contains(md5))
    {

        VideoData data = m_datas.value(md5);

        ui->label_Name->setText(data.name);
        ui->label_Description->setText(data.description);
        ui->label_Category->setText(data.category);
        ui->label_Author->setText(data.author);
        ui->label_Count->setText(QString::number(data.downloadCount));
    }
}

void OnlineClient::slotDoubleClickedChange(const QString &md5)
{
    slotClickedChange(md5);
    QString name = m_datas.value(m_currentMd5).name;
    QString newName = name.replace(QRegExp("\\s+"), "");
    QString saveFile = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene/"+newName;
    bool isExists = QFileInfo(saveFile).exists();
    if(!isExists)
    {
        QFuture<void> future = QtConcurrent::run([=](){
            bool bDown = downloadFileWithCurl(m_datas.value(m_currentMd5).downloadPath,
                                 saveFile);
            if(bDown && QFileInfo(saveFile).exists())
            {
                dApp->setWallPaper(saveFile);
                ui->label_Count->setText(QString::number(ui->label_Count->text().toInt()+1));
                m_client->sendData(u8"VIDEO_COUNT_ADD|"+m_currentMd5.toLatin1());
            }
            else
            {
                QMessageBox::information(nullptr, tr("Error"), tr("Dowlaod Error!"));
            }
        });

    }
    else
    {
        dApp->setWallPaper(saveFile);
    }

}


void OnlineClient::on_btn_search_clicked()
{
    QByteArray str = u8"GET_VIDEO_LIST|"+ui->edit_search->text().toUtf8()+"|1";
    m_client->sendData(str);
}

void OnlineClient::slotShowData(const QList<VideoData> &datas)
{
    m_datas.clear();
    for (VideoData data : datas)
    {
        m_datas.insert(data.md5,data);
    }
    m_viewDowload->setOnlienData(datas);
    int width = this->width();
    int height = this->height();
    this->resize(width+1,height+1);
    this->resize(width,height);
}


void OnlineClient::on_btn_Recommend_clicked()
{
    m_client->sendData(u8"GET_VIDEO_RECOMMEND|1");
}

