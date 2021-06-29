#ifndef CLIENTWIDGET_H
#define CLIENTWIDGET_H

#include <QWidget>
#include <QTcpSocket>//通信套接字
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWidget; }
QT_END_NAMESPACE

class ClientWidget : public QWidget
{
    Q_OBJECT

public:
    ClientWidget(QWidget *parent = nullptr);
    ~ClientWidget();

private slots:

    void on_ButtonSend_clicked();
    void on_ButtonClose_clicked();
    void on_ButtonConnect_clicked();

private:
    Ui::ClientWidget *ui;
    QTcpSocket *tcpSocket;

    QFile file;
    QString fileName;
    quint64 fileSize;//文件大小
    quint64 reciveSize;//已经接收文件大小

    bool isStart;
};
#endif // CLIENTWIDGET_H
