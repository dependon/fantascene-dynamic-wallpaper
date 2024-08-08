#include "tcpclient.h"
#include <QHostAddress>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QImage>

QByteArray extractBetweenMarkers(const QByteArray &data, const QByteArray &startMarker, const QByteArray &endMarker) {
    // 查找起始和结束标记的位置
    int startPos = data.indexOf(startMarker);
    if (startPos == -1) {
        // 没有找到起始标记，返回空
        return QByteArray();
    }

    // 起始标记后的位置（跳过标记本身）
    startPos += startMarker.length();

    // 查找结束标记的位置
    int endPos = data.indexOf(endMarker, startPos);
    if (endPos == -1) {
        // 没有找到结束标记，返回从起始标记到数据末尾的部分
        return data.mid(startPos);
    }

    // 提取并返回起始标记和结束标记之间的内容
    return data.mid(startPos, endPos - startPos);
}

TcpClient::TcpClient(const QString& host, quint16 port, QObject* parent)
    : QObject(parent), host(host), port(port), running(false)
{
    QThread * thread =new QThread();
    this->moveToThread(thread);
    thread->start();

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);

    qRegisterMetaType<VideoData>("VideoData");
    qRegisterMetaType<QList<VideoData>>("QList<VideoData>");
}

TcpClient::~TcpClient()
{
    stop();
    delete socket;
    socket = nullptr;
}

void TcpClient::run()
{


    running = true;
    socket->connectToHost(host, port);
    if (socket->waitForConnected(5000)) {
        Q_EMIT connected();
        qDebug() << "Connected to server";
    } else {
        Q_EMIT errorOccurred(socket->errorString());
        return;
    }
//    QFuture<void> future = QtConcurrent::run([=](){
//        while (running) {
//            queueMutex.lock();
//            if (dataQueue.isEmpty()) {
//                queueNotEmpty.wait(&queueMutex);
//            }
//            QByteArray data = dataQueue.dequeue();
//            queueMutex.unlock();

//            if (!data.isEmpty()) {
//                socket->write(data);
//                if (!socket->waitForBytesWritten(3000)) {
//                    Q_EMIT errorOccurred(socket->errorString());
//                }
//            }
//            QThread::msleep(100);
//        }

//        socket->disconnectFromHost();
//        if (socket->state() == QAbstractSocket::UnconnectedState ||
//                socket->waitForDisconnected(3000)) {
//            Q_EMIT disconnected();
//        }
//    });
    sendData(u8"GET_VIDEO_RECOMMEND|1");
}

void TcpClient::stop()
{
    running = false;
    queueNotEmpty.wakeAll();
}

void TcpClient::sendData(const QByteArray& data)
{
    socket->write(data);
    return ;
    QMutexLocker locker(&queueMutex);
    dataQueue.enqueue(data);
    queueNotEmpty.wakeOne();
}

bool TcpClient::TcpClient::isCompleteDataPacket()
{
    return dataBuffer.contains("TTTOVER") && dataBuffer.contains("TTTSTART");
}

void TcpClient::parseData(const QByteArray &data)
{
    QList<VideoData> datas;
    QList<QStringList> result;
    QStringList currentLine;

    // 将QByteArray转换为QString以方便处理
    QString dataStr(data);

    // 遍历每一行
    for (const QString &line : data.split('\n')) {
        // 跳过空行
        if (line.trimmed().isEmpty()) continue;

        // 检查是否为开始或结束标记
        if (line.startsWith("TTTSTART")) {
            // 可以选择在这里做一些初始化工作，但在这个例子中我们直接跳过
            continue;
        } else if (line.startsWith("TTTOVER")) {
            // 结束标记，跳出循环
            break;
        }

        // 否则，假设是数据行，按|分割
        currentLine = line.split('|');

        // 将分割后的数据添加到结果列表中
        result.append(currentLine);
    }
    int count = 0;
    for(QStringList list : result)
    {
        VideoData data;
        if(list.size() == 2)
        {
            count = list.at(1).toInt();
            Q_EMIT sigSearchTotalCount(count);
        }
        else if(list.size() >= 12)
        {
            data.md5 = list.at(0);
            data.name = list.at(1);
            data.description = list.at(2);
            data.category = list.at(3);
            data.author = list.at(4);
            data.fileName = list.at(5);
            data.downloadPath = list.at(6);
            data.downloadCount = list.at(7).toInt();
            data.filesize = list.at(8).toLongLong();
            data.width = list.at(9).toInt();
            data.height = list.at(10).toInt();
            data.picture = QByteArray::fromHex(list.at(11).toLatin1()); // Assuming picture is stored as a binary object
            datas << data;
        }

    }
    Q_EMIT sigShowData(datas);



}

void TcpClient::slotStart()
{
    run();
}

void TcpClient::onReadyRead()
{
    QByteArray dataAll = socket->readAll();
    dataBuffer.append(dataAll);
    // 处理接收到的数据，判断是否已经接收到完整的数据包
    if (isCompleteDataPacket())
    {
        QList<QByteArray> splitPackets; // 存储分割后的数据包
        int startIndex = 0;
        while (true) {
            // 查找TTTOVER的位置
            int eotIndex = dataBuffer.indexOf("TTTOVER", startIndex);
            if (eotIndex == -1) {
                // 没有找到TTTOVER，将剩余数据存入dataBuffer
                dataBuffer= dataBuffer.mid(startIndex);
                break;
            } else {
                // 找到TTTOVER，提取一个完整的数据包
                QByteArray packet = extractBetweenMarkers(dataBuffer, "TTTSTART", "TTTOVER");

                // 将数据包存入splitPackets列表
                splitPackets.append(packet);

                // 更新下一个数据包的开始位置
                startIndex = eotIndex + 1;
            }
        }
        for(QByteArray data : splitPackets)
        {
            parseData(data);
        }
    }

}

void TcpClient::onDisconnected()
{
    Q_EMIT disconnected();
    running = false;
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    Q_EMIT errorOccurred(socket->errorString());
}
