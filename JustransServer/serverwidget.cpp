#include "serverwidget.h"
#include "ui_serverwidget.h"
#include<QFileDialog>
#include<QMessageBox>
#include<QTimer>
#include<QFileInfo>
#include <QHostInfo>
#include <QNetworkInterface>


QString getIP() //获取ip地址
{
    QList<QHostAddress> list =QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() ==QAbstractSocket::IPv4Protocol)
           //我们使用IPv4地址
           return address.toString();
    }
    return 0;
}


ServerWidget::ServerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    //初始化套接字
    tcpServer = NULL;
    tcpSocket = NULL;

    //监听套接字，指定父对象自动回收空间
    tcpServer = new QTcpServer(this);
    //设定端口
    tcpServer->listen(QHostAddress::Any,8888);
    //新连接建立的信号槽
    connect(tcpServer,&QTcpServer::newConnection,[=](){
        //取出建立好连接的套接字
        tcpSocket = tcpServer->nextPendingConnection();
        //获取对方的ip和端口
        QString ip = tcpSocket->peerAddress().toString();
        qint16 port = tcpSocket->peerPort();

        //将信息显示到UI
        QString temp = QString("[%1:%2]: 连接成功").arg(ip).arg(port);
        ui->textEditRead->setText(temp);

        //连接建立后读取到数据的信号槽
        connect(tcpSocket,&QTcpSocket::readyRead,[=](){
            //从通信套接字取出内容
            QByteArray array = tcpSocket->readAll();
            //显示到UI
            ui->textEditRead->append(array);
        });
    });

    connect(&timer,&QTimer::timeout,[=](){
        //关闭定时器
        timer.stop();
        //发送文件
        sendData();
    });

    setWindowTitle("TCP服务器，端口：8888 " + getIP());
    qDebug()<<getIP();
}

ServerWidget::~ServerWidget()
{
    delete ui;
}

void ServerWidget::on_ButtonSend_clicked()
{
    qDebug()<<"send";
    if(NULL == tcpSocket)
    {
        return;
    }
    //获取编辑区内容
    QString str = ui->textEditWrite->toPlainText();
    //给对方发数据
    tcpSocket->write(str.toUtf8().data());
}
void ServerWidget::on_ButtonClose_clicked()
{
    if(NULL == tcpSocket)
    {
        return;
    }
    //主动和客户端断开连接
    tcpSocket->disconnectFromHost();
    //关闭套接字
    tcpSocket->close();
    tcpSocket = NULL;
}


//选择要发送的文件
void ServerWidget::on_ButtonFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"选择文件","../");
    //若文件路径无效。则提示
    if(filePath.isEmpty())
    {
        QMessageBox::information(this,"提示：",filePath+"文件路径无效");
        return ;
    }
    fileName.clear();
    fileSize=0;
    sendSize=0;
    //获取文件信息
    QFileInfo info(filePath);
    fileName = info.fileName();
    fileSize = info.size();

    //以只读方式打开
    //指定文件的名称
    file.setFileName(filePath);
    //打开文件
    bool isOK = file.open(QIODevice::ReadOnly);
    if(!isOK)
    {
        QMessageBox::information(this,"提示:","只读方式打开文件失败");
        return ;
    }

    //提示打开文件路径
    ui->textEditRead->append(filePath);
}


void ServerWidget::on_ButtonFileSend_clicked()
{
    //先发送文件头信息 文件名##文件大小
    QString head = QString("%1##%2").arg(fileName).arg(fileSize);
    //发送头部信息
    qint64 len = tcpSocket->write(head.toUtf8());
    //如果len>0，则头部信息发送成功
    if(len>0)
    {
        //发送真正的文件信息，为防止tcp黏包信息，定时器需要延时
        timer.start(20);
    }
    else
    {
        QMessageBox::information(this,"提示：","头部信息发送失败");
        file.close();
        return ;
    }

}

void ServerWidget::sendData()
{
    quint64 len = 0;
    do
    {
        //每次发送数据的大小
        char buf[1024] = {0};
        len=0;
        //往文件里读数据
        len = file.read(buf,sizeof(buf));
        //发送数据，读多少，发多少
        len = tcpSocket->write(buf,len);

        sendSize+=len;
    }while(len>0);
}
