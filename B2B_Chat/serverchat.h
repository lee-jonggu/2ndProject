#ifndef SERVERCHAT_H
#define SERVERCHAT_H

#include <QWidget>

namespace Ui {
class ServerChat;
}

class ServerChat : public QWidget
{
    Q_OBJECT

public:
    explicit ServerChat(int = 0, QWidget *parent = nullptr);
    ~ServerChat();

private:
    Ui::ServerChat *ui;

    int c_id;                                   // 서버와 1:1 채팅하는 클라이언트 id
    int client_id;                              // 데이터 관리를 위한 클라이언트 id

signals:
    void chatToClient(int,QString);             // 채팅창으로 부터 받은 메시지를 1:1 채팅창으로 옮기기 위한 시그널

public slots:
    void clientFromServer(QString,QString);     // 1:1 채팅창에 메시지를 띄우기 위한 함수
    void on_sendPushButton_clicked();           // 1:1 채팅창에서 send 버튼
private slots:
};

#endif // SERVERCHAT_H
