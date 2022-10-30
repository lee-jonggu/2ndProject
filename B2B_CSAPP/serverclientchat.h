#ifndef SERVERCLIENTCHAT_H
#define SERVERCLIENTCHAT_H

#include "chatserver.h"

#include <QWidget>

namespace Ui {
class ServerClientChat;
}

class ServerClientChat : public QWidget
{
    Q_OBJECT

public:
    explicit ServerClientChat(int = 0, QString = "", QWidget *parent = nullptr);
    ~ServerClientChat();

signals:
    void sendData(int,QString);                 // 고객사 ID, 이름 정보 전송

private slots:
    void on_sendPushButton_clicked();           // send 버튼
    void receiveData();                         // 고객사 채팅으로부터 받은 데이터 처리

public slots:
    void adminFromServer(QString,QString);      // 서버로부터 받은 관리자 채팅

private:
    Ui::ServerClientChat *ui;

    int c_id;                                   // 고객사 ID
    int client_id;                              // 고객사 ID 저장
    QString c_name;                             // 고객사 이름
};

#endif // SERVERCLIENTCHAT_H
