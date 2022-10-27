#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QWidget>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTreeWidgetItem>

class QFile;
class QProgressDialog;
class LogThread;
class ServerClientChat;

namespace Ui {
class ChatServer;
}

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

class ChatServer : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServer(QWidget *parent = nullptr);
    ~ChatServer();

signals:
//    void serverToChat(int,QString);                   // 서버와 1:1 채팅창에서 서버로 메시지 전달하는 함수
    void serverToClientChat(QString,QString);           // 클라이언트의 채팅을 서버로 부터 받는 시그널(이름, 데이터)

public slots:
    void removeClient();                                // 채팅창에서 클라이언트가 나간 경우의 슬롯 함수
    void showIdName(int,QString);                       // 클라이언트 정보에서 추가,삭제 되었을 경우 채팅 서버에 업데이트
    void removeIdName(int,int);                         // 클라이언트 리스트에서 고객 정보를 삭제할 경우 채팅 서버에서도 삭제
    void clientConnect();                               // 새로운 클라이언트가 연결
    void receiveData();                                 // 클라이언트가 보내는 프로토콜을 처리하는 함수

//    void showServerClient(QTreeWidgetItem*);            // 클라이언트 리스트에 저장된 클라이언트 정보를 채팅 서버에 출력
    void clientExpulsion();                             // 클라이언트 강퇴
    void clientAdmission();                             // 클라이언트 초대
    void acceptConnection();                            // 파일 서버 소켓 생성
    void readClient();                                  // 클라이언트에서 보낸 파일 받기
    void interactiveChat();                             // 서버와 클라이언트 간의 1:1 채팅

    void sendClientList();                              // 클라이언트에 채팅창 인원 보내기

    void serverToClient(int,QString);                   // 서버 관리자 채팅에서 서버로 클라이언트 ID, 메시지 전송


private slots:
    void on_enteredTreeWidget_customContextMenuRequested(const QPoint &pos);        // 채팅창 접속 클라이언트들에 대한 강퇴 메뉴

    void on_loginClientTreeWidget_customContextMenuRequested(const QPoint &pos);    // 서버 접속 클라이언트들에 대한 초대, 1:1 채팅 메뉴

private:
    Ui::ChatServer *ui;

    LogThread* logThread;                               // 로그 쓰레드

    QMenu *admissionMenu;                               // 채팅 초대 메뉴
    QMenu *expulsionMenu;                               // 채팅 강퇴 메뉴

    QTcpServer *tcpServer;                              // 채팅 서버
    QTcpServer *fileServer;                             // 파일 전송 서버
    QList<QTcpSocket*> clientList;                      // 서버에 접속한 클라이언트 리스트
    QList<QTcpSocket*> chatInClientList;                // 채팅에 접속한 클라이언트 리스트(소켓 이용)
    QList<QString> enterClientList;                     // 채팅에 접속한 클라이언트 리스트(이름 이용)

    QString clientId;                                   // 서버에 아이디가 저장되어 있는지 확인
    QString clientName;                                 // 서버에 이름이 저장되어 있는지 확인
    QHash<int, QString> clientNameHash;                 // port, name  해시
    QHash<QTcpSocket*, int> clientIdHash;               // socket, id  해시
    QHash<QString, QTcpSocket*> clientSocketHash;       // id, socket  해시
    QHash<int, ServerClientChat*> clientIdWindowHash;   // id, Window  해시
    QFile* file;                                        // 파일 전송을 위한 파일 객체
    QProgressDialog* progressDialog;                    // 파일 전송 현황을 보기 위한 프로그레스 다이얼로그
    qint64 totalSize;                                   // 전체 파일 크기
    qint64 byteReceived;                                // 받은 파일 크기
    QByteArray inBlock;                                 // 받은 파일을 바이트어레이 형태로 저장
    QByteArray outByteArray;                            // 채팅창에 클라이언트를 보여주기 위한 바이트어레이
};

#endif // CHATSERVER_H
