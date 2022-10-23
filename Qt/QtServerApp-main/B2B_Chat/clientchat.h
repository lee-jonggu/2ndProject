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

QT_BEGIN_NAMESPACE
namespace Ui { class ClientChat; }
QT_END_NAMESPACE

typedef enum {
    Server_In,
    Chat_In,
    Chat_Talk,
    Chat_Out,
    Server_Out,
    Chat_Expulsion,
    Chat_Admisson,
    Send_Client
} Chat_Status;

typedef struct {
    Chat_Status type;
    char data[1020];
} chatProtocolType;

class ClientChat : public QWidget
{
    Q_OBJECT

public:
    ClientChat(QWidget *parent = nullptr);
    ~ClientChat();


private slots:
    void disconnect();
    void receiveData();
    void sendData();
    void sendFile();
    void closeEvent(QCloseEvent*);
    void sendProtocol(Chat_Status, char*, int = 1020);
    void goOnSend(qint64);

    void on_sendButton_clicked();

    void on_lineEdit_returnPressed();

    void on_connectPushButton_clicked();

    void on_chatPushButton_clicked();

    void on_disConnectPushButton_clicked();

    void on_chatOutPushButton_clicked();

    void on_fileTransferPushButton_clicked();

private:
    Ui::ClientChat *ui;

    QLineEdit *id;
    QLineEdit *name;
    QTextEdit *message;
    QLineEdit *inputLine;
    QTcpSocket *clientSocket;                               // 채팅을 위한 소켓
    QList<QTcpSocket*> clientList;
    QTcpSocket *fileSocket;                                 // 파일 전송을 위한 소켓
    QProgressDialog* progressDialog;                        // 파일 전송 확인
    QFile* file;                    // 서버로 보내는 파일
    qint64 loadSize;                // 파일의 크기
    qint64 byteToWrite;             // 보내는 파일의 크기
    qint64 totalSize;               // 전체 파일의 크기
    QByteArray outBlock;            // 전송을 위한 데이터
    bool isSent;                    // 파일 서버에 접속되었는지 확인
};
#endif // CLIENTCHAT_H
