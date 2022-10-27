#include "serverclientchat.h"
#include "ui_serverclientchat.h"

#define BLOCK_SIZE  1024

ServerClientChat::ServerClientChat(int c_id, QString c_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerClientChat)
{
    qDebug() << c_id << c_name;
    client_id = c_id;
    ui->setupUi(this);
}

ServerClientChat::~ServerClientChat()
{
    delete ui;
}

void ServerClientChat::on_sendPushButton_clicked()
{
    QString str = ui->inputLineEdit->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        ui->chatTextEdit->append("관리자 : " + str);
    }
    emit sendData(client_id,str);
}

void ServerClientChat::receiveData()
{
//    QTcpSocket *clientConnection = (QTcpSocket *)sender();
//    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
//    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

//    chatProtocolType data;                                          // 프로토콜타입 유형
//    memset(data.data,0,1020);
//    QDataStream in(&bytearray, QIODevice::ReadOnly);
//    in >> data.type;
//    in.readRawData(data.data, 1020);

//    switch(data.type){
//    case Chat_Talk:
////        ui->chatTextEdit->append(QString(data.data));
//        break;
//    }
}

void ServerClientChat::adminFromServer(QString name,QString str)
{
    qDebug() << name << str;
    if(str.length())
    {
        ui->chatTextEdit->append(name + " : " + str);
    }
}
