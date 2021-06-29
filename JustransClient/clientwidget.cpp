#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>

ClientWidget::ClientWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientWidget)
{
    ui->setupUi(this);

    ui->progressBar->setValue(0);
    isStart = true;

    //初始化套接字
    tcpSocket = NULL;
    //分配空间，指定父对象
    tcpSocket = new QTcpSocket(this);
    //建立连接的信号槽
    connect(tcpSocket,&QTcpSocket::connected,[=](){
        ui->textEditRead->setText("恭喜，成功连接服务器");
    });

    //建立连接后收到服务器数据的信号槽
//    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
//        //获取对方发送的数据
//        QByteArray array = tcpSocket->readAll();
//        ui->textEditRead->append(array);
//    });

    //建立连接后收到服务器文件数据的信号槽
    connect(tcpSocket,&QTcpSocket::readyRead,[=](){
        //获取对方发送的文件数据
        QByteArray buf = tcpSocket->readAll();
        if(isStart==true)
        {
            isStart = false;
            //解析头部信息,初始化
            fileName = QString(buf).section("##",0,0);
            fileSize = QString(buf).section("##",1,1).toInt();
            reciveSize = 0;

            //打开文件
            file.setFileName("./"+fileName);
            bool isOK = file.open(QIODevice::WriteOnly|QIODevice::Append);
            if(isOK == false)
            {
//                tcpSocket->disconnectFromHost();
//                tcpSocket->close();
                QMessageBox::information(this,"提示：","打开文件错误");
//                return ;
            }

            //初始化进度条
            ui->progressBar->setMinimum(0);
            ui->progressBar->setMaximum(fileSize/1024);
            ui->progressBar->setValue(0);
        }

        else
        {
            //文件信息
            quint64 len = file.write(buf);
            if(len>0)
            {
                reciveSize+=len;
            }

            //更新进度条
            ui->progressBar->setValue(reciveSize/1024);
            if(reciveSize==fileSize)
            {
                //传输文件完毕
                file.close();
                QMessageBox::information(this,"提示：","文件接收完毕");
//                tcpSocket->disconnectFromHost();
//                tcpSocket->close();
            }
        }
    });

    setWindowTitle(QString("TCP客户端"));
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_ButtonSend_clicked()
{
    //获取编辑内容
    QString str = ui->textEditWrite->toPlainText();
    //发送数据
    tcpSocket->write(str.toUtf8().data());
}

void ClientWidget::on_ButtonClose_clicked()
{
    tcpSocket->disconnectFromHost();
}

void ClientWidget::on_ButtonConnect_clicked()
{
    //获取服务器IP和端口
    QString ip = ui->lineEditIP->text();
    qint16 port = ui->lineEditPort->text().toInt();

    //主动连接服务器
    tcpSocket->connectToHost(QHostAddress(ip),port);
    ui->progressBar->setValue(0);
}
