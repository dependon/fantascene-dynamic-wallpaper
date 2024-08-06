#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QThread>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "data.h"
class TcpClient : public QThread
{
    Q_OBJECT

public:
    TcpClient(const QString& host, quint16 port, QObject* parent = nullptr);
    ~TcpClient();

    void run() override;
    void stop();
    void sendData(const QByteArray& data);

    bool isCompleteDataPacket();

    //解析数据
    void parseData(const QByteArray& data);
Q_SIGNALS:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& errorString);
    void sigShowData(const QList<VideoData>& datas);
public Q_SLOTS:
    void onReadyRead();
    void onDisconnected();

    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket* socket;
    QString host;
    quint16 port;
    QQueue<QByteArray> dataQueue;
    QMutex queueMutex;
    QWaitCondition queueNotEmpty;
    bool running;

    QByteArray dataBuffer;//缓存
};

#endif // TCPCLIENT_H
