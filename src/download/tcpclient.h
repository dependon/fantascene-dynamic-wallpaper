#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QThread>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include "data.h"
class TcpClient : public QObject
{
    Q_OBJECT

public:
    TcpClient(const QString& host, quint16 port, QObject* parent = nullptr);
    ~TcpClient();

    void run() ;
    void stop();

    bool isCompleteDataPacket();

    //解析数据
    void parseData(const QByteArray& data);
Q_SIGNALS:
    void connected();
    void disconnected();
    void dataReceived(const QByteArray& data);
    void errorOccurred(const QString& errorString);
    void sigShowData(const QList<VideoData>& datas);
    void sigSearchTotalCount(const int & count);
public Q_SLOTS:
    void slotStart();
    void onReadyRead();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void sendData(const QByteArray& data);

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
