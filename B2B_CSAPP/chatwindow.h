#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include "chatserver.h"

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void echoData();                                                // 서버로부터 받은 데이터 처리
    void sendData();                                                // 서버로 보내는 1:1 채팅창 데이터
    void sendProtocol(Chat_Status type, char* data, int = 1020);    // 서버로 보내는 프로토콜과 데이터

    void on_pushButton_clicked();                                   // send 버튼
    void on_lineEdit_returnPressed();                               // 라인에딧에서 엔터

private:
    Ui::ChatWindow *ui;

    QTextEdit *message;                                             // 채팅창에 보여지는 메시지
    QLineEdit *inputLine;                                           // 전송할 메시지
    QTcpSocket *clientSocket;                                       // 전송을 위한 소켓
    QList<QTcpSocket*> clientList;                                  // 소켓을 통한 클라이언트 리스트 관리
};

#endif // CHATWINDOW_H
