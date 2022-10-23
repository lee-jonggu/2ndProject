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
    Send_Client
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

    void sendClientList();                          // 클라이언트에 채팅창 인원 보내기


private slots:
    void on_enteredTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_loginClientTreeWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::ChatServer *ui;

    LogThread* logThread;

    QMenu *admissionMenu;
    QMenu *expulsionMenu;

    QLabel *infoLabel;
    QTcpServer *tcpServer;
    QTcpServer *fileServer;
    QList<QTcpSocket*> clientList;

    QList<QString> enterClientList;

    QString clientId;  // 서버에 아이디가 저장되어 있는지 확인
    QString clientName;

    QHash<int, QString> clientNameHash;             // port, name
//    QHash<QString, int> clientIdHash;
    QHash<QTcpSocket*, int> clientIdHash;           // socket, id
    QHash<QString, QTcpSocket*> clientSocketHash;
    QFile* file;
    QProgressDialog* progressDialog;
    qint64 totalSize;
    qint64 byteReceived;
    QByteArray inBlock;
};

#endif // CHATSERVER_H
