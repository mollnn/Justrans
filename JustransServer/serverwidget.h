#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include<QTcpServer>//监听套接字
#include<QTcpSocket>//通信套接字
#include<QFile>
#include<QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWidget; }
QT_END_NAMESPACE

class ServerWidget : public QWidget
{
    Q_OBJECT

public:
    ServerWidget(QWidget *parent = nullptr);
    ~ServerWidget();

    void sendData();//发送数据

public slots:
    void on_ButtonSend_clicked();
    void on_ButtonClose_clicked();

    void on_ButtonFile_clicked();
    void on_ButtonFileSend_clicked();

private:
    Ui::ServerWidget *ui;
    QTcpServer *tcpServer;//监听套接字
    QTcpSocket *tcpSocket;//通信套接字

    QFile file;//文件对象
    QString fileName;//文件名称
    quint64 fileSize;//文件大小
    quint64 sendSize;//已经发送文件大小
    QTimer timer;//定时器

};
#endif // SERVERWIDGET_H
