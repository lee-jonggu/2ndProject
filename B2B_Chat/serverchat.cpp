#include "serverchat.h"
#include "ui_serverchat.h"

ServerChat::ServerChat(int c_id , QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerChat)
{
    ui->setupUi(this);

    client_id = c_id;                       // 데이터 관리를 위한 클라이언트 id 설정
}

ServerChat::~ServerChat()
{
    delete ui;
}

void ServerChat::clientFromServer(QString c_name,QString str)   // 서버로부터 1:1 채팅창으로 온 메시지
{
    if(str.length())
    {
        ui->chatTextEdit->append("관리자 : " + str);             // 서버에서 보낸 메시지 화면에 출력
    }
}

void ServerChat::on_sendPushButton_clicked()
{
    QString str = ui->inputLineEdit->text();                    // 1:1 채팅창에서 관리자에게 보내기 위한 메시지
    if(str.length())
    {
        ui->chatTextEdit->append("나 : " + str);                // 1:1 채팅창에서 보낸 메시지 화면에 출력
    }

    emit(chatToClient(client_id,str));                          // 서버로 메시지를 보내기 위한 시그널
}

