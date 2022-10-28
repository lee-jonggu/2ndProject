#include "chatwindow.h"
#include "ui_chatwindow.h"

#include <QWidget>

#define BLOCK_SIZE  1024

ChatWindow::ChatWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow)
{
    ui->setupUi(this);

    clientSocket = new QTcpSocket(this);                                // 새로운 소켓 생성
    clientSocket->connectToHost("127.0.0.1",8010);                      // 서버 관리자 채팅이기 때문에 로컬 호스트로 바로 연결
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(echoData()));   // 데이터가 들어있는 소켓을 받으면 함수 실행
}

ChatWindow::~ChatWindow()
{
    clientSocket->close();                                              // 창이 꺼질때 소켓 소멸
}

void ChatWindow::on_pushButton_clicked()                                // send 버튼
{
    sendData();                                                         // 서버로 메시지 전송
    ui->inputLine->clear();                                             // 인풋에딧 초기화
}


void ChatWindow::on_lineEdit_returnPressed()                            // 인풋에딧을 채우고 엔터
{
    sendData();                                                         // 서버로 메시지 전송
    ui->inputLine->clear();                                             // 인풋에딧 초기화
}

void ChatWindow::echoData()                                             // 서버로 부터 데이터를 받을 경우
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();              // 서버와 통신하는 소켓 특정
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;        // 받는 데이터가 처리할 수 있는 데이터보다 많다면 리턴
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);          // 바이트어레이로 데이터 읽기
    foreach(QTcpSocket *sock, clientList) {                             // 서버에 접속되어 있는 소켓 검사
        if (sock != clientConnection)                                   // 연결되어있는 소켓 중 현재 소켓을 빼고
            sock->write(bytearray);                                     // 다른 소켓에 전송
    }
    ui->textEdit->append(QString(bytearray));                           // 채팅창에 메시지 출력
}

void ChatWindow::sendData()                                             // 서버로 전송하는 메시지
{
    QString str = ui->inputLine->text();                                // 서버로 보낼 메시지를 QString으로 설정
    if(str.length())                                                    // 보낼 메시지가 있다면
    {
        QByteArray bytearray;                                           // 서버로 보낼 바이트어레이
        bytearray = str.toUtf8();                                       // 바이트어레이의 타입을 UTF-8로 설정
        ui->textEdit->append("관리자 : " + str);                         // 채팅창에 메시지 출력
        sendProtocol(Manager_Chat, bytearray.data());                   // 서버에 프로토콜 타입, 데이터 전송
    }
}

void ChatWindow::sendProtocol(Chat_Status type, char* data, int size)   // 서버로 보내는 프로토콜 타입, 데이터
{
    QByteArray sendArray;                                               // 서버로 보낼 바이트어레이
    QDataStream out(&sendArray, QIODevice::WriteOnly);                  // 바이트어레이를 담을 데이터스트림을 쓰기 전용으로 열기
    out.device()->seek(0);                                              // 데이터스트림의 커서를 제일 앞 쪽으로 이동
    out << type;                                                        // 프로토콜 타입 담기
    out.writeRawData(data, size);                                       // 소켓에 데이터 담기
    clientSocket->write(sendArray);                                     // 소켓을 통해 서버로 전송
    clientSocket->flush();                                              // 보낸 소켓을 초기화
    while(clientSocket->waitForBytesWritten());                         // 소켓에 데이터가 다시 담길때까지 대기
}
