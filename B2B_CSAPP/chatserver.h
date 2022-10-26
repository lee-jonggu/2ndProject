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

typedef enum {
    Server_In,
    Chat_In,
    Chat_Talk,
    Chat_Out,
    Server_Out,
    Chat_Expulsion,
    Chat_Admission,
    Send_Client,
    Manager_Chat,        // 서버 채팅 전송 프로토콜
    Chat_One
} Chat_Status;

typedef struct {
    Chat_Status type;
    char data[1020];
} chatProtocolType;

class ChatServer : public QWidget
{
    Q_OBJECT

public:
    explicit ChatServer(QWidget *parent = nullptr);
    ~ChatServer();

signals:
    void serverToChat(int,QString);

public slots:
    void removeClient();
    void showIdName(int,QString);
    void removeIdName(int,int);
    void clientConnect();
    void receiveData();

    void showServerClient(QTreeWidgetItem*);
    void clientExpulsion();                               // 클라이언트 강퇴
    void clientAdmission();                                // 클라이언트 초대
    void acceptConnection();                        // 파일 서버 소켓 생성
    void readClient();                              // 클라이언트에서 보낸 파일 받기
    void interactiveChat();

    void sendClientList();                          // 클라이언트에 채팅창 인원 보내기

    void serverToClient(int,QString);

//    void sendProtocol(Chat_Status , char* , int = 1020);


private slots:
    void on_enteredTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_loginClientTreeWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ChatServer *ui;

//    ServerClientChat *serverClientChat;

    LogThread* logThread;

    QMenu *admissionMenu;
    QMenu *expulsionMenu;

    QLabel *infoLabel;
    QTcpServer *tcpServer;
    QTcpServer *fileServer;
    QList<QTcpSocket*> clientList;
    QList<QTcpSocket*> chatInClientList;
    QList<QString> enterClientList;

    QString clientId;  // 서버에 아이디가 저장되어 있는지 확인
    QString clientName;

//    QList<QTreeWidgetItem*> chatInClientList;                    // 채팅창에 참여한 클라이언트
    QHash<int, QString> clientNameHash;             // port, name
//    QHash<QString, int> clientIdHash;
    QHash<QTcpSocket*, int> clientIdHash;           // socket, id
    QHash<QString, QTcpSocket*> clientSocketHash;
    QFile* file;
    QProgressDialog* progressDialog;
    qint64 totalSize;
    qint64 byteReceived;
    QByteArray inBlock;
    QByteArray outByteArray;                        // 채팅창에 클라이언트를 보여주기 위한 바이트어레이
};

#endif // CHATSERVER_H
