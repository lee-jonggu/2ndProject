#include "serverclientchat.h"
#include "ui_serverclientchat.h"

#define BLOCK_SIZE  1024

ServerClientChat::ServerClientChat(int c_id, QString c_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerClientChat)
{
    client_id = c_id;                                   // 클라이언트 ID 설정
    ui->setupUi(this);
}

ServerClientChat::~ServerClientChat()
{
    delete ui;
}

void ServerClientChat::on_sendPushButton_clicked()      // send 버튼
{
    QString str = ui->inputLineEdit->text();            // 입력창에 입력한 메시지
    if(str.length())                                    // 가 있다면
    {
        QByteArray bytearray;                           // 메시지 전송을 위한 바이트 어레이
        bytearray = str.toUtf8();                       // 바이트어레이를 UTF-8 변환
        ui->chatTextEdit->append("관리자 : " + str);     // 채팅창에 출력
    }
    ui->inputLineEdit->clear();                         // 입력창 초기화
    emit sendData(client_id,str);                       // 클라이언트 ID와 메시지 넘겨주기
}

void ServerClientChat::adminFromServer(QString name,QString str)    // 서버로 부터 받은 메시지
{
    if(str.length())                                                // 메시지가 있다면
    {
        ui->chatTextEdit->append(name + " : " + str);               // 채팅창에 출력
    }
}
