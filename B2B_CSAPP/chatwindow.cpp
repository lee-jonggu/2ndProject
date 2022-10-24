#include "chatwindow.h"
#include "ui_chatwindow.h"

#include <QWidget>

#define BLOCK_SIZE  1024

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    clientSocket = new QTcpSocket(this);
    clientSocket->connectToHost("127.0.0.1",8010);
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(echoData()));
}

ChatWindow::~ChatWindow()
{
    clientSocket->close();
}

void ChatWindow::on_pushButton_clicked()
{
    sendData();
    ui->inputLine->clear();
}


void ChatWindow::on_lineEdit_returnPressed()
{
    sendData();
    ui->inputLine->clear();
}

void ChatWindow::echoData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);
    foreach(QTcpSocket *sock, clientList) {
        if (sock != clientConnection)
            sock->write(bytearray);
    }
    ui->textEdit->append(QString(bytearray));
}

void ChatWindow::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        clientSocket->write(bytearray);
    }
    ui->textEdit->append("나 : " + str);
}
