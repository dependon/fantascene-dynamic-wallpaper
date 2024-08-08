#ifndef ONLINECLIENT_H
#define ONLINECLIENT_H

#include <QWidget>
#include <QMap>
#include <QMutex>
#include "data.h"
class TcpClient;
class view;
namespace Ui {
class OnlineClient;
}

class OnlineClient : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineClient(QWidget *parent = nullptr);
    ~OnlineClient();

    static bool downloadFileWithCurl(const QString &url, const QString &outputFilePath,const QString& extraPath = "");

    QString fileSizeToMString(const qint64 &fileNameSize);
    QString removeZipSuffix(const QString &str);

Q_SIGNALS:
    void sigStart();
    void sigSendData(const QByteArray& data);
private Q_SLOTS:
    void on_btn_search_clicked();
    void slotShowData(const QList<VideoData>& datas);
    void on_btn_download_clicked();
    void slotClickedChange(const QString & md5);
    void slotDoubleClickedChange(const QString & md5);
    void on_btn_Recommend_clicked();

    void slotSearchTotalCount(const int & count);

    void on_btn_Right_clicked();

    void on_btn_Left_clicked();

private:
    Ui::OnlineClient *ui;
    view *m_viewDowload{nullptr};
    TcpClient* m_client{nullptr};
    QMap<QString , VideoData> m_datas;
    QString m_currentMd5;
    bool m_isRecommd{true};
    QString m_searchString;
    QMutex m_downloadMutex;
};

#endif // ONLINECLIENT_H
