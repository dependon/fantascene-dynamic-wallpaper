#include "onlineclient.h"
#include "ui_onlineclient.h"
#include "view.h"
#include "tcpclient.h"
#include <QProcess>
#include <QStandardPaths>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>
#include <QMutexLocker>
#include "application.h"

OnlineClient::OnlineClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineClient)
{
    ui->setupUi(this);
    setWindowTitle(tr("Fantascene Hub"));
    m_viewDowload = new view(this);
    ui->verticalLayout->addWidget(m_viewDowload);

    m_client = new TcpClient("192.168.40.48",26667);
    connect(this,&OnlineClient::sigStart,m_client,&TcpClient::slotStart,Qt::QueuedConnection);
    connect(this,&OnlineClient::sigSendData,m_client,&TcpClient::sendData,Qt::QueuedConnection);
    Q_EMIT sigStart();

    QString path = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene";

    if (!QDir(path).exists()) {
        // 路径不存在，尝试创建路径
        QDir dir;
        if (dir.mkpath(path)) {
            qDebug() << "Path created:" << path;
        }
    }
    connect(m_client,&TcpClient::sigShowData,this,&OnlineClient::slotShowData);
    connect(m_client,&TcpClient::sigSearchTotalCount,this,&OnlineClient::slotSearchTotalCount);

    connect(m_viewDowload,&view::sigClickedChange,this,&OnlineClient::slotClickedChange);
    connect(m_viewDowload,&view::sigDoubleClicked,this,&OnlineClient::slotDoubleClickedChange);

    ui->label_tip1->setWordWrap(true);
    ui->label_tip2->setWordWrap(true);
}

OnlineClient::~OnlineClient()
{
    delete ui;
}

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

void OnlineClient::on_btn_download_clicked()
{
    QString name = m_datas.value(m_currentMd5).fileName;
    QString newName = name.replace(QRegExp("\\s+"), "");
    QString saveFile = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene/"+newName;

    QFuture<void> future = QtConcurrent::run([=]()
    {
        QMutexLocker locker(&m_downloadMutex);
        ui->label_DTip->setText(tr("Dowload Ing....."));
        bool isExists = QFileInfo(saveFile).exists();
        if(!isExists)
        {
            ui->btn_download->setEnabled(false);
            bool bDown = downloadFileWithCurl(m_datas.value(m_currentMd5).downloadPath,
                                              saveFile);
            if(bDown && QFileInfo(saveFile).exists())
            {
                ui->label_Count->setText(QString::number(ui->label_Count->text().toInt()+1));
                Q_EMIT sigSendData(u8"VIDEO_COUNT_ADD|"+m_currentMd5.toLatin1());
            }
            else
            {
                QMessageBox::information(nullptr, tr("Error"), tr("Dowlaod Error!"));
            }
            ui->btn_download->setEnabled(true);
        }
        else
        {
            QMessageBox::information(nullptr, tr("Error"), tr("File Exists!"));
        }
        ui->label_DTip->setText("");
    });
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
        ui->label_FileName->setText(data.fileName);
        ui->label_FileSize->setText(QString::number(data.filesize));
        ui->label_Width->setText(QString::number(data.width));
        ui->label_Height->setText(QString::number(data.height));
    }
}

void OnlineClient::slotDoubleClickedChange(const QString &md5)
{
    slotClickedChange(md5);
    QString name = m_datas.value(m_currentMd5).fileName;
    QString newName = name.replace(QRegExp("\\s+"), "");
    QString saveFile = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)+"/fantascene/"+newName;
    QFuture<void> future = QtConcurrent::run([=]()
    {
        QMutexLocker locker(&m_downloadMutex);
        ui->label_DTip->setText(tr("Dowload Ing....."));
        bool isExists = QFileInfo(saveFile).exists();
        if(!isExists)
        {
            ui->btn_download->setEnabled(false);
            bool bDown = downloadFileWithCurl(m_datas.value(m_currentMd5).downloadPath,
                                              saveFile);
            if(bDown && QFileInfo(saveFile).exists())
            {
                dApp->setWallPaper(saveFile);
                ui->label_Count->setText(QString::number(ui->label_Count->text().toInt()+1));
                Q_EMIT sigSendData(u8"VIDEO_COUNT_ADD|"+m_currentMd5.toLatin1());
            }
            else
            {
                QMessageBox::information(nullptr, tr("Error"), tr("Dowlaod Error!"));
            }
            ui->btn_download->setEnabled(true);
        }
        else
        {
            QMessageBox::information(nullptr, tr("Error"), tr("File Exists!"));
        }
        ui->label_DTip->setText("");
    });

}


void OnlineClient::on_btn_search_clicked()
{
    m_isRecommd = false;
    QByteArray str = u8"GET_VIDEO_LIST|"+ui->edit_search->text().toUtf8()+"|1";
    m_searchString = ui->edit_search->text();
    Q_EMIT sigSendData(str);
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
    Q_EMIT sigSendData(u8"GET_VIDEO_RECOMMEND|1");
}

void OnlineClient::slotSearchTotalCount(const int &count)
{
    int totalPage = count/20+1;
    ui->label_Total->setText(QString::number(totalPage));
    ui->label_CurrentCount->setText(QString::number(1));
}


void OnlineClient::on_btn_Right_clicked()
{
    int current = ui->label_CurrentCount->text().toInt();
    int totalPage = ui->label_Total->text().toInt();
    current +=1;
    if(current <= 1 || current > totalPage)
    {
        return ;
    }
    if(m_isRecommd)
    {
        QByteArray str = u8"GET_VIDEO_RECOMMEND|" + QString::number(current).toUtf8();
        Q_EMIT sigSendData(str);
    }
    else
    {
        QByteArray str = u8"GET_VIDEO_LIST|"+ m_searchString.toUtf8() + "|" + QString::number(current).toUtf8();
        Q_EMIT sigSendData(str);
    }
    ui->label_CurrentCount->setText(QString::number(current));
}


void OnlineClient::on_btn_Left_clicked()
{
    int current = ui->label_CurrentCount->text().toInt();
    current -=1;
    int totalPage = ui->label_Total->text().toInt();
    if(current < 1 || current > totalPage)
    {
        return ;
    }

    if(m_isRecommd)
    {
        QByteArray str = u8"GET_VIDEO_RECOMMEND|" + QString::number(current).toUtf8();
        Q_EMIT sigSendData(str);
    }
    else
    {
        QByteArray str = u8"GET_VIDEO_LIST|"+ m_searchString.toUtf8() + "|" + QString::number(current).toUtf8();
        Q_EMIT sigSendData(str);
    }
    ui->label_CurrentCount->setText(QString::number(current));
}

