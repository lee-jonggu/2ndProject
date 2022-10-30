#include "clientchat.h"
#include "serverchat.h"
#include "ui_clientchat.h"

#include <QByteArray>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFile>
#include <QFileDialog>

#define BLOCK_SIZE  1024

ClientChat::ClientChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientChat) , isSent(false)
{
    ui->setupUi(this);

    ui->inputLine->setDisabled(true);                                       // 채팅에 접속하기 전까지 비활성화

    ui->ipAddressLineEdit->setText("127.0.0.1");                            // 기본 ip 주소
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");       // ip 주소 입력을 위한 정규 표현식
    QRegularExpressionValidator validator(re);                              // 정규 표현식
    ui->ipAddressLineEdit->setPlaceholderText("Server IP Address");         // 비어있을 경우 표시되는 텍스트
    ui->ipAddressLineEdit->setValidator(&validator);                        // 정규 표현식 적용

    ui->portLineEdit->setText("8010");                                      // 기본 port 번호
    ui->portLineEdit->setInputMask("00000;_");                              // 숫자만 입력
    ui->portLineEdit->setPlaceholderText("Server Port No");                 // port 번호 입력

    ui->clientIdLineEdit->setInputMask("0000;_");                           // 클라이언트 id 숫자만 입력
    ui->clientIdLineEdit->setPlaceholderText("ID No");                      // 클라이언트 id 입력
    ui->clientNameLineEdit->setPlaceholderText("Your Name");                // 클라이언트 이름 입력

    ui->chatPushButton->setDisabled(true);                                  // chat_in 버튼 비활성화
    ui->chatOutPushButton->setDisabled(true);                               // chat_out 버튼 비활성화
    ui->disConnectPushButton->setDisabled(true);                            // disconnect 버튼 비활성화

    clientSocket = new QTcpSocket(this);                                    // 채팅을 위한 소켓
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(receiveData()));    // 소켓을 통해 데이터가 오면 데이터 처리
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));    // 소켓이 끊겼을 경우 처리

    fileSocket = new QTcpSocket(this);                                      // 파일 전송을 위한 소켓
    connect(fileSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));  // 파일 소켓이 다 써지면 보낼 준비
    progressDialog = new QProgressDialog(0);                                // 프로그레스 다이얼로그 초기화
    progressDialog->setAutoClose(true);                                     // 프로그레스 다이얼로그 숨기기
    progressDialog->reset();                                                // 프로그레스 다이얼로그 재시작
}

ClientChat::~ClientChat()
{
    clientSocket->close();

    delete ui;
}

void ClientChat::disconnect()                                               // 서버 연결이 끊겼을 경우
{
    QMessageBox::critical(this, tr("Chatting Client"), tr("Disconnect from Server"));
    ui->chatPushButton->setDisabled(true);                                  // chat_in 버튼 비활성화
    ui->chatOutPushButton->setDisabled(true);                               // chat_out 버튼 비활성화
    ui->disConnectPushButton->setDisabled(true);                            // disconnect 버튼 비활성화
}

void ClientChat::on_sendButton_clicked()                                    // send 버튼 눌렀을 경우
{
    sendData();                                                             // 데이터 보내기
    ui->inputLine->clear();                                                 // 인풋라인 초기화
}


void ClientChat::on_lineEdit_returnPressed()                                // 메시지 보낼때 엔터 눌렀을 경우
{
    sendData();                                                             // 데이터 보내기
    ui->inputLine->clear();                                                 // 인풋라인 초기화
}

void ClientChat::receiveData()                                              // 서버로부터 데이터 받았을 경우
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();                  // 서버와 통신하는 소켓 특정
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;            // 보낸 데이터가 처리할 수 있는 데이터보다 많다면 리턴
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);              // 처리할 수 있는 데이터 수 만큼 데이터 읽기

    chatProtocolType data;                                                  // 프로토콜타입의 타입과 데이터
    memset(data.data,0,1020);                                               // 프로토콜 타입을 제외한 1020 byte 초기화
    QDataStream in(&bytearray, QIODevice::ReadOnly);                        // 받은 데이터스트림을 읽기 전용으로 열기
    in >> data.type;                                                        // 프로토콜 타입 받기
    in.readRawData(data.data, 1020);                                        // 보낸 데이터 받기
    qDebug() << data.type;
    switch(data.type){                                                      // 프로토콜 타입에 따른 데이터 처리
    case Chat_Talk:                                                         // 받은 프로토콜이 Chat_Talk 이면
        foreach(QTcpSocket *sock, clientList) {                             // 서버에 접속한 클라이언트 리스트 소켓 조사
            if (sock != clientConnection)                                   // 서버와 연결된 소켓 중 내가 아닌 소켓한테
                sock->write(bytearray);                                     // 데이터 전송
        }
        ui->textEdit->append(QString(data.data));                           // 채팅창에 데이터 표시
        break;
    case Chat_Expulsion:                                                    // 강퇴 프로토콜 타입
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Expulsion from Server"));
        ui->inputLine->setDisabled(true);                                   // 보낼 메시지 비활성화
        ui->chatOutPushButton->setDisabled(true);                           // 채팅 나가기 비활성화
        ui->chatPushButton->setDisabled(false);                             // 채팅 들어가기 활성화
        ui->disConnectPushButton->setDisabled(false);                       // 서버 나가기 활성화
        ui->treeWidget->clear();                                            // 채팅창을 나간 경우와 같으므로 접속한 클라이언트 리스트 초기화
        break;
    case Chat_Admission:                                                    // 초대 프로토콜 타입
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Admissioned from Server"));
        ui->inputLine->setDisabled(false);                                  // 보낼 메시지 활성화
        ui->chatPushButton->setDisabled(true);                              // 채팅 들어가기 비활성화
        ui->chatOutPushButton->setDisabled(false);                          // 채팅 나가기 활성화
        ui->disConnectPushButton->setDisabled(true);                        // 서버 나가기 비활성화
        ui->connectPushButton->setDisabled(true);                           // 서버 들어가기 비활성화
        ui->clientIdLineEdit->setReadOnly(true);                            // 접속한 id 고정
        ui->clientNameLineEdit->setReadOnly(true);                          // 접속한 이름 고정
        break;
    case Send_Client: {                                                     // 채팅창 접속 클라이언트 리스트 업데이트
        ui->treeWidget->clear();                                            // 현재 채팅창 클라이언트 리스트 초기화
        QString str = data.data;                                            // 채팅창에 있는 클라이언트 리스트 이름
        for(int i=0;i<str.split("/").size();i++)                            // 구분자를 이용하여 클라이언트 구분
        {
            QTreeWidgetItem *item = new QTreeWidgetItem;                    // 채팅창 접속 리스트에 추가하기 위한 트리위젯아이템
            item->setText(0,str.split("/")[i]);                             // 구분자를 이용하여 클라이언트 구분
            ui->treeWidget->addTopLevelItem(item);                          // 채팅창 접속 리스트에 추가
        }
        /* 클라이언트 리스트를 구분자로 구분할 경우 마지막은 공백이 나오기 때문에 마지막 아이템은 제거 */
        ui->treeWidget->takeTopLevelItem(ui->treeWidget->findItems("",Qt::MatchContains).count()-1);
    }
        break;

    case Manager_Chat:                                                      // 관리자가 전달하는 전체 메시지
        foreach(QTcpSocket *sock, clientList) {                             // 서버에 접속한 모든 클라이언트 소켓 검사
            if (sock != clientConnection)                                   // 특정 소켓이 받고 모든 소켓에 뿌려준다
                sock->write(bytearray);                                     // 소켓을 통해 다시 데이터를  다른 클라이언트들에게 전송
        }
        ui->textEdit->append(QString(data.data));                           // 채팅창에 메시지 표시

        break;

    case Chat_One:                                                          // 관리자와 1:1 채팅
        QString str = QString(data.data);                                   // 받은 메시지 QString 으로 저장
        QString c_name = ui->clientNameLineEdit->text();                    // 메시지를 받은 클라이언트 이름
        emit(serverToClient(c_name,str));                                   // 1:1 채팅창에 메시지를 띄우기 위한 시그널
        break;
    }
}

void ClientChat::sendData()                                                 // 클라이언트가 서버로 채팅 전송
{
    QString str = ui->inputLine->text();                                    // 전송할 데이터 저장
    if(str.length())                                                        // 전송할 채팅이 있다면
    {
        QByteArray bytearray;                                               // 서버로 보낼 바이트어레이
        bytearray = str.toUtf8();                                           // 보낼 데이터 타입을 UTF-8로 저장
        ui->textEdit->append("나 : " + str);                                // 채팅을 보낸 클라이언트 채팅창에 보낸 메시지 표시

        sendProtocol(Chat_Talk, bytearray.data());                          // 서버로 프로토콜 타입과 데이터 전송
    }
}


void ClientChat::on_connectPushButton_clicked()                             // connect 버튼
{
    /* 접속할 ip와 port번호로 접속 시도 */
    clientSocket->connectToHost(ui->ipAddressLineEdit->text(), ui->portLineEdit->text().toInt());
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});

    chatProtocolType data;                                                  // 서버로 데이터 전송을 위한 프로토콜 타입
    data.type = Server_In;                                                  // Server_In 프로토콜 타입 담기
    qstrcpy(data.data, ui->clientIdLineEdit->text().toStdString().data());  // 보낼 데이터를 타입에 맞춰 변환
    QByteArray sendArray;                                                   // 서버로 보낼 바이트어레이
    QDataStream out(&sendArray, QIODevice::WriteOnly);                      // 서버로 보낼 데이터스트림을 쓰기 전용으로 열기
    out << data.type;                                                       // 프로토콜 타입 담기
    out.writeRawData(data.data, 1020);                                      // 보낼 데이터를 소켓에 담기
    clientSocket->write(sendArray);                                         // 소켓을 통해 서버로 전송

    ui->connectPushButton->setDisabled(true);                               // 서버 들어가기 버튼 비활성화
    ui->connectPushButton->setText("connected");                            // 서버 들어가기 버튼 서버 접속중 으로 변환
    ui->disConnectPushButton->setDisabled(false);                           // 서버 나가기 버튼 활성화

    ui->chatPushButton->setDisabled(false);                                 // 채팅 들어가기 버튼 활성화

    ui->ipAddressLineEdit->setReadOnly(true);                               // 접속한 ip 고정
    ui->portLineEdit->setReadOnly(true);                                    // 접속한 port 번호 고정
    ui->clientIdLineEdit->setReadOnly(true);                                // 접속한 클라이언트 id 고정
    ui->clientNameLineEdit->setReadOnly(true);                              // 접속한 클라이언트 이름고정
}

void ClientChat::sendProtocol(Chat_Status type, char* data, int size)       // 서버로 프로토콜 타입, 데이터 보내기
{
    QByteArray sendArray;                                                   // 서버로 보낼 바이트어레이
    QDataStream out(&sendArray, QIODevice::WriteOnly);                      // 보낼 데이터스트림을 쓰기 전용으로 열기
    out.device()->seek(0);                                                  // 데이터스트림의 커서를 제일 앞 쪽으로 이동
    out << type;                                                            // 프로토콜 타입 담기
    out.writeRawData(data, size);                                           // 소켓 데이터 채우기
    clientSocket->write(sendArray);                                         // 소켓을 통해 서버로 전송
    clientSocket->flush();                                                  // 전송한 소켓 초기화
    while(clientSocket->waitForBytesWritten());                             // 다음 전송까지 대기
}

void ClientChat::on_chatPushButton_clicked()                                // Chat_In 버튼
{
    /* 서버로 프로토콜, 데이터 보내기 */
    sendProtocol(Chat_In, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(false);                                      // 보낼 메시지 활성화
    ui->chatPushButton->setDisabled(true);                                  // 채팅 들어가기 비활성화
    ui->chatOutPushButton->setDisabled(false);                              // 채팅 나가기 활성화
    ui->disConnectPushButton->setDisabled(true);                            // 서버 나가기 비활성화
    ui->connectPushButton->setDisabled(true);                               // 서버 들어가기 비활성화
    ui->clientIdLineEdit->setReadOnly(true);                                // 접속한 id 고정
    ui->clientNameLineEdit->setReadOnly(true);                              // 접속한 이름 고정
    ui->textEdit->setText("<font size=4>채팅창에 입장하셨습니다.</font>");     // 채팅창에 접속하면 접속 메시지 출력
}

void ClientChat::on_chatOutPushButton_clicked()                             // Chat_Out 버튼
{
    /* 서버로 프로토콜, 데이터 보내기 */
    sendProtocol(Chat_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(true);                                       // 보낼 메시지 비활성화
    ui->chatOutPushButton->setDisabled(true);                               // 채팅 나가기 활성화
    ui->chatPushButton->setDisabled(false);                                 // 채팅 들어가기 활성화
    ui->disConnectPushButton->setDisabled(false);                           // 서버 나가기 활성화
    ui->treeWidget->clear();                                                // 채팅창 나간 경우 이므로 채팅 접속 클라이언트 초기화
}

void ClientChat::on_disConnectPushButton_clicked()                          // disconnect 버튼
{
    /* 서버로 프로토콜, 데이터 보내기 */
    sendProtocol(Server_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->disConnectPushButton->setDisabled(true);                            // 서버 나가기 비활성화
    ui->connectPushButton->setDisabled(false);                              // 서버 들어가기 활성화
    ui->chatPushButton->setDisabled(true);                                  // 채팅 들어가기 비활성화
    ui->clientIdLineEdit->setReadOnly(false);                               // 접속할 id 입력 가능
    ui->clientNameLineEdit->setReadOnly(false);                             // 접속할 이름 입력 가능
    ui->ipAddressLineEdit->setReadOnly(false);                              // 접속할 ip 입력 가능
    ui->portLineEdit->setReadOnly(false);                                   // 접속할 port 입력 가능
    ui->treeWidget->clear();                                                // 채팅 접속 클라이언트 리스트 초기화
}

void ClientChat::on_fileTransferPushButton_clicked()                        // 파일 전송 버튼
{
    sendFile();                                                             // 파일 전송
    ui->fileTransferPushButton->setDisabled(true);                          // 파일 전송 버튼 비활성화
}

void ClientChat::sendFile()                                                 // 파일 전송
{
    loadSize = 0;                                                           // 파일 크기 초기화
    byteToWrite = 0;                                                        // 보내는 파일 크기 초기화
    totalSize = 0;                                                          // 전체 파일의 크기 초기화
    outBlock.clear();                                                       // 전송을 위한 데이터 초기화

    QString filename = QFileDialog::getOpenFileName(this);                  // 파일 경로
    if(filename.length()) {                                                 // 파일 경로가 있다면
        file = new QFile(filename);                                         // 파일 경로 만들기
        file->open(QFile::ReadOnly);                                        // 파일 경로 열기

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0);                                        // 프로그레스 다이얼로그 초기화

        if (!isSent) {                                                      // 파일을 처음 보내는 거라면
            fileSocket->connectToHost(ui->ipAddressLineEdit->text( ),       // ip 주소와
                                      ui->portLineEdit->text( ).toInt( ) + 1);  // port 번호로 접속
            isSent = true;                                                  // 파일을 보낸 이력이 있다
        }

        /* 이전에 파일을 보냈다면 */
        byteToWrite = totalSize = file->size();                             // 보내는 파일의 크기와 전체 크기 파일 크기 설정
        loadSize = 1024;                                                    // 보내는 파일의 최대 크기

        QDataStream out(&outBlock, QIODevice::WriteOnly);                   // 데이터스트림을 쓰기 전용으로 열기
        /* 보낼 파일의 크기를 정해야 하므로 파일 경로+이름, 보내는 클라이언트 이름을 제외한 크기는 임시로 설정 */
        out << qint64(0) << qint64(0) << filename << ui->clientNameLineEdit->text() << ui->clientIdLineEdit->text();
        totalSize += outBlock.size();           // 전체 파일의 크기는 전송 하는 데이터만큼 추가
        byteToWrite += outBlock.size();         // 보내는 파일의 크기는 전송 하는 데이터만큼 추가

        out.device()->seek(0);                  // 데이터스트림의 커서를 제일 앞 쪽으로 이동
        out << totalSize << qint64(outBlock.size());    // 앞쪽부터 두개의 스트림에 전체 파일 크기, 전송을 위한 데이터 크기 설정

        fileSocket->write(outBlock);            // 소켓을 통해 서버로 데이터 전송

        progressDialog->setMaximum(totalSize);  // 보내는 파일의 크기 만큼 다이얼로그 최대값 설정
        progressDialog->setValue(totalSize-byteToWrite);    // 보내는 파일이 얼만큼 갔는지 표시
        progressDialog->show();                 // 다이얼로그 표시
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

void ClientChat::goOnSend(qint64 numBytes)      // 파일 나눠서 보내기
{
    byteToWrite -= numBytes;                    // 보내는 파일 크기 만큼 전체 크기에서 빼주기
    outBlock = file->read(qMin(byteToWrite, numBytes)); // 파일을 나눠서 전송
    fileSocket->write(outBlock);                // 나눈 파일을 각각 전송

    progressDialog->setMaximum(totalSize);      // 보낼 파일의 최대 크기로 다이얼로그 최대값 설정
    progressDialog->setValue(totalSize-byteToWrite);    // 보내는 파일 현황 보기

    if (byteToWrite == 0) {                     // 파일을 다 보냈다면
        qDebug("File sending completed!");
        progressDialog->reset();                // 다이얼로그 리셋
    }
}

void ClientChat::closeEvent(QCloseEvent*)
{
//    sendProtocol(Server_Out, name->text().toStdString().data());
//    clientSocket->disconnectFromHost();
//    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
//        clientSocket->waitForDisconnected();
}

void ClientChat::on_serverPushButton_clicked()                  // 서버 관리자와 1:1 채팅
{
    int c_id = ui->clientIdLineEdit->text().toInt();            // 관리자와 1:1 채팅하는 클라이언트 id
    ServerChat *w = new ServerChat(c_id);                       // 1:1 채팅창 생성
    QString c_name = ui->clientNameLineEdit->text();            // 1:1 채팅창 제목을 위한 클라이언트 이름
    w->setWindowTitle(QString::number(c_id) + " " + c_name);    // 1:1 채팅창 제목 설정
    w->show();                                                  // 1:1 채팅창 보여주기

    /* 서버로 부터 받은 1:1 채팅창 메시지를 클라이언트 1:1 채팅창으로 전송 */
    connect(this,SIGNAL(serverToClient(QString,QString)),w,SLOT(clientFromServer(QString,QString)));
    /* 1:1 채팅창으로 부터 받은 메시지를 서버로 전송 */
    connect(w,SIGNAL(chatToClient(int,QString)),this,SLOT(clientToServer(int,QString)));
}

void ClientChat::clientToServer(int c_id,QString str)           // 1:1 채팅창 메시지를 서버로 전송
{
    sendProtocol(Chat_One, str.toStdString().data());           // 프로토콜 타입과 데이터 전송
}

