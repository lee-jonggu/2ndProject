#ifndef CLIENTCHAT_H
#define CLIENTCHAT_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>

class QProgressDialog;
class QFile;
class ServerChat;

QT_BEGIN_NAMESPACE
namespace Ui { class ClientChat; }
QT_END_NAMESPACE

typedef enum {                                          // 서버 채팅 전송 프로토콜
    Server_In,                                          // 클라이언트 서버 접속 프로토콜
    Chat_In,                                            // 클라이언트 채팅 접속 프로토콜
    Chat_Talk,                                          // 클라이언트 간 채팅 프로토콜
    Chat_Out,                                           // 클라이언트 채팅 아웃 프로토콜
    Server_Out,                                         // 클라이언트 서버 아웃 프로토콜
    Chat_Expulsion,                                     // 클라이언트 채팅 강퇴 프로토콜
    Chat_Admission,                                     // 클라이언트 채팅 초대 프로토콜
    Send_Client,                                        // 채팅에 접속한 클라이언트 명단 전송 프로토콜
    Manager_Chat,                                       // 서버에서 관리자 메시지 전송 프로토콜
    Chat_One                                            // 관리자와 클라이언트 간 1:1 채팅 프로토콜
} Chat_Status;

typedef struct {                                        // 프로토콜 타입 구조체
    Chat_Status type;                                   // 프로토콜 타입
    char data[1020];                                    // 데이터
} chatProtocolType;

class ClientChat : public QWidget
{
    Q_OBJECT

public:
    ClientChat(QWidget *parent = nullptr);
    ~ClientChat();

signals:
    void serverToClient(QString,QString);               // 클라이언트 채팅창에서 1:1 채팅창으로 시그널

private slots:
    void disconnect();                                  // 서버가 끊긴 경우
    void receiveData();                                 // 서버에서 받은 데이터 처리
    void sendData();                                    // 서버로 보내는 채팅
    void sendFile();                                    // 서버로 보내는 파일
    void closeEvent(QCloseEvent*);                      // 채팅창이 꺼졌을 경우 이벤트
    void sendProtocol(Chat_Status, char*, int = 1020);  // 서버로 보내는 프로토콜 타입, 데이터
    void goOnSend(qint64);                              // 서버로 파일을 보낼 준비
    void clientToServer(int,QString);                   // 1:1 채팅의 아이디와 텍스트를 서버로 전송

    void on_sendButton_clicked();                       // send 버튼
    void on_lineEdit_returnPressed();                   // 라인에딧에서 엔터
    void on_connectPushButton_clicked();                // connect 버튼
    void on_chatPushButton_clicked();                   // chat_in 버튼
    void on_disConnectPushButton_clicked();             // disconnect 버튼
    void on_chatOutPushButton_clicked();                // chat_out 버튼
    void on_fileTransferPushButton_clicked();           // fileTransfer 버튼
    void on_serverPushButton_clicked();                 // server_in 버튼

private:
    Ui::ClientChat *ui;

    QLineEdit *id;                                      // 서버에 접속하는 클라이언트 id
    QLineEdit *name;                                    // 서버에 접속하는 클라이언트 이름
    QTextEdit *message;                                 // 채팅창에 보이는 메시지
    QLineEdit *inputLine;                               // 서버에 보내는 메시지
    QTcpSocket *clientSocket;                           // 채팅을 위한 소켓
    QList<QTcpSocket*> clientList;                      // 소켓을 통한 클라이언트 리스트 관리
    QTcpSocket *fileSocket;                             // 파일 전송을 위한 소켓
    QProgressDialog* progressDialog;                    // 파일 전송 확인
    QFile* file;                                        // 서버로 보내는 파일
    qint64 loadSize;                                    // 파일의 크기
    qint64 byteToWrite;                                 // 보내는 파일의 크기
    qint64 totalSize;                                   // 전체 파일의 크기
    QByteArray outBlock;                                // 전송을 위한 데이터
    bool isSent;                                        // 파일 서버에 접속되었는지 확인
};
#endif // CLIENTCHAT_H
