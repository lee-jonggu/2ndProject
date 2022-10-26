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
    // 여기서만 Send_Client 프로토콜만 준다면????
}

void ChatWindow::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        ui->textEdit->append("관리자 : " + str);

//        sendProtocol(Chat_Talk, bytearray.data());
        sendProtocol(Manager_Chat, bytearray.data());
    }
}

void ChatWindow::sendProtocol(Chat_Status type, char* data, int size)
{
    QByteArray sendArray;           // 소켓으로 보낼 데이터를 채우고
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(sendArray);     // 서버로 전송
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}
