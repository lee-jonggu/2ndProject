#include "chatserver.h"
#include "logthread.h"
#include "serverclientchat.h"
#include "ui_chatserver.h"

#include <QtGui>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFileDialog>
#include <QProgressDialog>
#include <QListWidgetItem>

#define BLOCK_SIZE  1024
#define PORT_NUMBER 8010

ChatServer::ChatServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatServer), totalSize(0), byteReceived(0)
{
    ui->setupUi(this);                                                      // 현재 클래스에 ui form을 올린다

    tcpServer = new QTcpServer(this);                                       // 채팅을 위한 서버
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));      // 채팅 서버로 새로운 클라이언트 연결 시그널 & 슬롯
    if (!tcpServer->listen(QHostAddress::Any, PORT_NUMBER))                 // 어느 ip에서나 접속 가능, 포트번호는 8010 고정
    {
        // 채팅 서버가 시작되지 않았을 시 오류
        QMessageBox::critical(this, tr("Echo Server"),tr("Unable to start the server : %1.").arg(tcpServer->errorString()));
        return;
    }

    fileServer = new QTcpServer(this);                                      // 파일 전송을 위한 서버
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection())); // 파일 서버로 새로운 클라이언트 연결
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {            // 파일 서버의 포트 번호는 채팅 서버의 +1
        // 파일 서버가 시작되지 않았을 시 오류
        QMessageBox::critical(this, tr("Chatting Server"),tr("Unable to start the server: %1.").arg(fileServer->errorString()));
        return;
    }

    QAction* expulsionAction = new QAction(tr("&Expulsion"));               // 컨텍스트 메뉴에 사용할 강퇴 액션
    connect(expulsionAction, SIGNAL(triggered()), SLOT(clientExpulsion())); // 강퇴 액션을 실행했을 경우 슬롯 함수 실행
    expulsionMenu = new QMenu;                                              // 강퇴 메뉴 생성
    expulsionMenu->addAction(expulsionAction);                              // 메뉴바에 강퇴 액션 추가
    ui->enteredTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);     // 요청 시그널을 보내기 위한 정보 추가


    QAction* admissionAction = new QAction(tr("&Addmission"));              // 컨텍스트 메뉴에 사용할 초대 액션
    connect(admissionAction, SIGNAL(triggered()),SLOT(clientAdmission()));  // 초대 액션을 실행했을 경우 슬롯 함수 실행
    admissionMenu = new QMenu;                                              // 초대 메뉴 생성
    admissionMenu->addAction(admissionAction);                              // 메뉴바에 초대 액션 추가
    QAction* serverClientInteractiveAction = new QAction(tr("&One on One Chat"));   // 컨텍스트 메뉴에 사용할 1:1 채팅 액션
    // 1:1 채팅 액션이 눌려지면 1:1 채팅 함수 실행
    connect(serverClientInteractiveAction, SIGNAL(triggered()),SLOT(interactiveChat()));
    admissionMenu->addAction(serverClientInteractiveAction);                // 메뉴바에 1:1 채팅 액션 추가
    ui->loginClientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu); // 요청 시그널을 보내기 위한 정보 추가

    progressDialog = new QProgressDialog(0);                                // 프로그레스 다이얼로그 0 으로 시작
    progressDialog->setAutoClose(true);                                     // 다이얼이 0 이면 자동으로 닫힌다
    progressDialog->reset();                                                // 다이얼로그 재시작

    logThread = new LogThread(this);                                        // 로그 저장을 위한 로그쓰레드 생성
    logThread->start();                                                     // 서버가 시작될 때 로그쓰레그 실행

    connect(ui->logPushButton, SIGNAL(clicked()), logThread, SLOT(saveData())); // 로그 저장 버튼을 누르면 로그를 파일로 저장
    qDebug() << "Saved the log file.";
}

ChatServer::~ChatServer()
{
    delete ui;

    logThread->terminate();                                                 // 서버가 종료될 때 로그 쓰레드 종료
    tcpServer->close( );                                                    // 서버가 종료될 때 채팅 서버 종료
    fileServer->close( );                                                   // 서버가 종료될 때 파일 서버 종료
}

void ChatServer::clientConnect()                                            // 새로운 클라이언트가 생길 때마다 함수 실행
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();      // 소켓을 만들어 놓고 데이터가 올때마다 연결
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));// 서버와 클라이언트의 연결이 끊기면 서버에서 클라이언트 제거
    connect(clientConnection, SIGNAL(readyRead()),this,SLOT(receiveData()));// 클라이언트에서 데이터를 보내면 서버에서 데이터를 받는다.
}

void ChatServer::receiveData()                                              // 클라이언트에서 받아오는 데이터 처리
{

    QTcpSocket *clientConnection = (QTcpSocket *)sender();                  // 클라이언트와 연결된 소켓 찾기

    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;            // 받아오는 데이터가 처리할 수 있는 데이터 보다 많으면 리턴
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);              // 블록 사이즈만큼 바이트어레이에 저장

    chatProtocolType data;                                                  // 프로토콜 타입과 데이터 받기

    QDataStream in(&bytearray, QIODevice::ReadOnly);                        // 받은 데이터를 읽기 전용으로 받는다
    in >> data.type;                                                        // 프로토콜 타입 추출
    in.readRawData(data.data, 1020);                                        // 프로토콜 데이터 추출

    QString ip = clientConnection->peerAddress().toString();                // 소켓과 연결된 ip 저장
    quint16 port = clientConnection->peerPort();                            // 소켓과 연결된 port 저장
    QString id = QString::fromStdString(data.data);                         // 데이터를 QString 형태로 저장

    /* 클라이언트 id에 맞는 이름을 찾는다 */
    foreach(auto item, ui->totalClientTreeWidget->findItems(id,Qt::MatchFixedString,0))
    {
        clientName = item->text(1); // 찾은 이름을 멤버 변수로 저장
    }

    switch(data.type) {             // 프로토콜 타입에 따라 처리하기 위한 switch 문
    case Server_In:                 // 프로토콜 타입이 Server_In 이면,
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))   // 클라이언트 id를 이용하여
        {
            /* 서버에 접속한 클라이언트 표시를 위한 트리위젯 아이템 */
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
            enterItem->setText(0,id);                               // 서버에 접속한 클라이언트 id 표시
            enterItem->setText(1,clientName);                       // 서버에 접속한 클라이언트 이름 표시
            ui->loginClientTreeWidget->addTopLevelItem(enterItem);  // 서버에 접속한 클라이언트 정보 출력
            clientList.append(clientConnection);                    // 클라이언트 리스트에 맞는 소켓 저장
            clientSocketHash[clientName] = clientConnection;        // 서버에 접속한 클라이언트 이름을 키로하는 소켓 저장 해시
            clientIdHash[clientConnection] = id.toInt();            // 서버에 접속한 클라이언트의 소켓을 key로 하는 id 저장 해시
            clientId = id;                                          // 클라이언트 id를 멤버 변수로 저장

            QTreeWidgetItem *items = new QTreeWidgetItem;           // 로그 표시를 위한 트리위젯아이템
            items->setText(0,ip);                                   // 서버에 접속한 소켓의 ip 주소
            items->setText(1,QString::number(port));                // 서버에 접속한 소켓의 port 번호
            items->setText(2,QString::number(clientIdHash[clientConnection]));  // 서버에 접속한 소켓의 id 번호
            items->setText(3,clientNameHash[port]);                 // 서버에 접속한 소켓의 클라이언트 이름
            items->setText(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");                                // 서버에 접속한 소켓의 데이터를 로그에 찍기
            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");                                // 로그 메시지가 길 경우 툴팁으로 표시
            items->setText(5,QDateTime::currentDateTime().toString());  // 로그를 찍는 시간
            ui->logTreeWidget->addTopLevelItem(items);              // 트리위젯에 로그 표시
            logThread->appendData(item);                            // 로그 쓰레드에 로그 추가
        }
        break;
    case Chat_In:                                                   // 프로토콜 타입이 Chat_In 이라면,
    {
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))   // 클라이언트 id를 이용하여,
        {
            /* 채팅에 접속한 클라이언트 표시를 위한 트리위젯 아이템 */
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
            QString id = item->text(0);                             // 서버에 접속한 클라이언트 아이디를 통해 채팅창에 표시
            QString name = item->text(1);                           // 서버에 접속한 클라이언트 이름을 통해 채팅창에 표시
            enterItem->setText(0,id);                               // 채팅에 접속한 클라이언트 id 표시
            enterItem->setText(1,clientName);                       // 채팅에 접속한 클라이언트 이름 표시
            ui->enteredTreeWidget->addTopLevelItem(enterItem);      // 채팅에 접속한 클라이언트 정보 출력
            clientNameHash[port] = name;                            // port, name 해시에 저장
            chatInClientList.append(clientConnection);              // 채팅에 접속한 클라이언트를 리스트에 추가

            QTreeWidgetItem *items = new QTreeWidgetItem;           // 로그 표시를 위한 트리위젯아이템
            items->setText(0,ip);                                   // 채팅에 접속한 소켓의 ip 주소
            items->setText(1,QString::number(port));                // 채팅에 접속한 소켓의 port 번호
            items->setText(2,clientId);                             // 채팅에 접속한 소켓의 id 번호
            items->setText(3,clientName);                           // 채팅에 접속한 소켓의 클라이언트 이름
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");                              // 채팅에 접속한 소켓의 이름을 로그에 찍기
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");                              // 로그 메시지가 길 경우 툴팁으로 표시
            items->setText(5,QDateTime::currentDateTime().toString());  // 로그를 찍는 시간
            ui->logTreeWidget->addTopLevelItem(items);              // 트리위젯에 로그 표시
            logThread->appendData(item);                            // 로그 쓰레드에 로그 추가

        }
        sendClientList();                                           // 채팅에 접속한 클라이언트 리스트 업데이트
    }
        break;
    case Chat_Talk: {                                               // 클라이언트간 채팅
        foreach(QTcpSocket *sock, clientList) {                     // 서버에 접속한 클라이언트 소켓을 검사
            if(clientNameHash.contains(sock->peerPort()) && sock != clientConnection) { // 내가 아닌 다른 소켓들에게 전송
                QByteArray sendArray;                               // 보낼 데이터를 담을 바이트어레이
                sendArray.clear();                                  // 바이트어레이 초기화
                QDataStream out(&sendArray, QIODevice::WriteOnly);  // 바이트어레이를 데이터스트림에 쓰기 전용으로 담아서 열기
                out << Chat_Talk;                                   // 프로토콜 타입 담기
                sendArray.append(clientNameHash[port].toStdString().data());    // 보낼 데이터에 보내는 클라이언트 이름 담기
                sendArray.append(" : ");                            // 구분자
                sendArray.append(id.toStdString().data());          // 보낼 데이터 담기
                sock->write(sendArray);                             // 소켓을 통해 다른 클라이언트에게 전송
            }
        }

        QTreeWidgetItem *item = new QTreeWidgetItem;                // 로그 표시를 위한 트리위젯아이템
        item->setText(0,ip);                                        // 데이터를 전송한 소켓의 ip 주소
        item->setText(1,QString::number(port));                     // 데이터를 전송한 소켓의 port 번호
        item->setText(2,clientId);                                  // 데이터를 전송한 소켓의 클라이언트 id 번호
        item->setText(3,clientNameHash[port]);                      // 데이터를 전송한 소켓의 클라이언트 이름
        item->setText(4,QString(data.data));                        // 데이터를 전송한 소켓의 데이터를 로그에 찍기
        item->setToolTip(4,QString(data.data));                     // 로그 메시지가 길 경우 툴팁으로 표시
        item->setText(5,QDateTime::currentDateTime().toString());   // 로그를 찍은 시간
        ui->logTreeWidget->addTopLevelItem(item);                   // 트리위젯에 로그 표시
        logThread->appendData(item);                                // 로그 쓰레드에 로그 추가
    }
        break;
    case Chat_Out:                                                  // 프로토콜 타입이 Chat_Out 이라면,
        foreach(auto item, ui->enteredTreeWidget->findItems(id, Qt::MatchFixedString, 0))   // 클라이언트 id를 이용하여
        {
            /* id를 이용하여 채팅에 접속한 트리위젯에서 클라이언트 제거 */
            ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
            clientNameHash.remove(port);                            // Chat_In할때 들어왔던 해시에서 삭제

            if(!chatInClientList.isEmpty()){                        // 클라이언트 리스트가 비어져있지 않다면
                QList<QTcpSocket*>::Iterator eraseSock;             // 이터레이터를 이용하여 소켓을 삭제
                /* 서버에 접속한 클라이언트 리스트를 돌면서 소켓 찾기 */
                for(auto sock = chatInClientList.begin(); chatInClientList.end() != sock; sock++){
                    if(*sock == clientConnection){                  // 현재 데이터를 보내는 소켓이 맞다면
                        eraseSock = sock;                           // 해당 소켓 제거를 위해 멤버 변수로 지정
                    }
                }
                chatInClientList.erase(eraseSock);                  // 해당 소켓 삭제
            }

            QTreeWidgetItem *items = new QTreeWidgetItem;           // 로그 표시를 위한 트리위젯 아이템
            items->setText(0,ip);                                   // 채팅창을 나간 소켓의 ip 주소
            items->setText(1,QString::number(port));                // 채팅창을 나간 소켓의 port 번호
            items->setText(2,clientId);                             // 채팅창을 나간 소켓의 클라이언트 id 번호
            items->setText(3,clientName);                           // 채팅창을 나간 소켓의 클라이언트 이름
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");                             // 채팅창을 나간 소켓의 클라이언트 정보 메시지
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");                             // 메시지가 길 경우 툴팁으로 표시
            items->setText(5,QDateTime::currentDateTime().toString());  // 로그를 찍는 시간
            ui->logTreeWidget->addTopLevelItem(items);              // 트리위젯에 로그 표시
            logThread->appendData(item);                            // 로그 쓰레드에 로그 추가
        }
        sendClientList();                                           // 채팅창에 접속한 클라이언트를 갱신
        break;

    case Server_Out:                                                // 프로토콜 타입이 Server_Out 이라면,
        /* 클라이언트 id를 이용하여 */
        foreach(auto item, ui->loginClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            /* 서버를 나간 클라이언트 삭제 - 트리위젯의 인덱스를 이용 */
            ui->loginClientTreeWidget->takeTopLevelItem(ui->loginClientTreeWidget->indexOfTopLevelItem(item));
            clientSocketHash.remove(clientName);                    // Server_In할 때 들어왔던 해시에서 삭제

            QTreeWidgetItem *items = new QTreeWidgetItem;           // 로그 표시를 위한 트리위젯아이템
            items->setText(0,ip);                                   // 서버를 나간 소켓의 ip 주소
            items->setText(1,QString::number(port));                // 서버를 나간 소켓의 port 번호
            items->setText(2,clientId);                             // 서버를 나간 소켓의 클라이언트 id
            items->setText(3,clientName);                           // 서버를 나간 소켓의 클라이언트 이름
            items->setText(4,"ID : " + clientId + " Name : " +clientName\
                    + " Server Out");                               // 서버를 나간 소켓의 클라이언트 정보 메시지
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Server Out");                               // 메시지가 길 경우 툴팁으로 표시
            items->setText(5,QDateTime::currentDateTime().toString());  // 로그를 찍는 시간
            ui->logTreeWidget->addTopLevelItem(items);              // 트리위젯에 로그 표시
            logThread->appendData(item);                            // 로그 쓰레드에 로그 추가
        }
        sendClientList();                                           // 채팅창에 접속한 클라이언트를 갱신
        break;

    case Manager_Chat: {                                            // 프로토콜타입이 Manager_Chat 이라면,
        foreach(QTcpSocket *sock, clientList) {                     // 서버에 접속한 클라이언트 소켓을 검사
            if(clientNameHash.contains(sock->peerPort()) && sock != clientConnection) { // 내가 아닌 다른 소켓들에게 전송
                QByteArray sendArray;                               // 보낼 데이터를 담을 바이트어레이
                sendArray.clear();                                  // 바이트어레이 초기화
                QDataStream out(&sendArray, QIODevice::WriteOnly);  // 바이트어레이를 데이터스트림에 쓰기 전용으로 담아서 열기
                out << Chat_Talk;                                   // 프로토콜 타입 담기
                sendArray.append("<font color = darkorange font-weight= bold>");    // 폰트 설정
                sendArray.append("관리자 : ");                       // 관리자 표시
                sendArray.append(id.toStdString().data());          // 보낼 데이터 담기
                sendArray.append("</font>");                        // 폰트 설정
                sock->write(sendArray);                             // 소켓을 통해 전송
            }
        }
        break;
    }
    case Chat_One: {                                                // 프로토콜 타입이 Chat_One 이라면,
        QTcpSocket *sock = clientConnection;                        // 데이터를 보낸 소켓 특정
        int c_id = clientIdHash[sock];                              // 소켓으로 클라이언트 id 추출
        /* 채팅에 접속한 전체 리스트 */
        foreach(auto item, ui->loginClientTreeWidget->findItems(QString::number(c_id),Qt::MatchContains)) {
            QString name = item->text(1);                               // 채팅에 접속한 리스트로 클라이언트 이름 추출
            clientIdWindowHash[c_id]->adminFromServer(name,data.data);  // 서버의 관리자 1:1 채팅에 받은 데이터 업데이트
        }
    }
        break;
    }
}

void ChatServer::showIdName(int id,QString name)                        // 서버에 접속한 클라이언트 리스트 표시
{
    QTreeWidgetItem *item = new QTreeWidgetItem;                        // 리스트 추가를 위한 트리위젯 아이템
    item->setText(0,QString::number(id));                               // 클라이언트 id
    item->setText(1,name);                                              // 클라이언트 이름
    ui->totalClientTreeWidget->addTopLevelItem(item);                   // 서버에 접속한 클라이언트 리스트 출력
}

void ChatServer::removeIdName(int id,int index)                         // 클라이언트 id를 인덱스로 활용하여 서버 접속 클라이언트 제거
{
    Q_UNUSED(id);                                                       // 특정 id 받아와서 쓰지 않기
    ui->totalClientTreeWidget->takeTopLevelItem(index);                 // 인덱스를 이용하여 서버 접속 클라이언트 제거
}

//void ChatServer::showServerClient(QTreeWidgetItem* item)
//{
//    ui->totalClientTreeWidget->addTopLevelItem(item);
//}

void ChatServer::clientExpulsion()                                      // 클라이언트 강퇴
{
    QByteArray sendArray;                                               // 보낼 데이터를 담을 바이트어레이
    QDataStream out(&sendArray, QIODevice::WriteOnly);                  // 바이트어레이를 데이터스트림에 쓰기 전용으로 담아서 열기
    out << Chat_Expulsion;                                              // 프로토콜 타입 담기
    out.writeRawData("", 1020);                                         // 타입만 보내고 데이터는 보내지 않는다

    QString id = ui->enteredTreeWidget->currentItem()->text(0);         // 로그를 찍기 위한 클라이언트 id
    QString name = ui->enteredTreeWidget->currentItem()->text(1);       // 로그를 찍기 위한 클라이언트 이름
    QTcpSocket* sock = clientSocketHash[name];                          // 클라이언트 이름을 이용해서 소켓 특정
    quint16 port = sock->peerPort();                                    // 강퇴된 클라이언트의 port 번호
    QString ip = sock->peerAddress().toString();                        // 로그를 찍기 위한 클라이언트 ip
    sock->write(sendArray);                                             // 소켓을 통해 데이터 전송
    /* 클라이언트 id를 이용하여 */
    foreach(auto item, ui->enteredTreeWidget->findItems(id, Qt::MatchFixedString, 0))
    {
        /* 강퇴된 클라이언트를 채팅창에서 삭제 */
        ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
    }
    QTreeWidgetItem *items = new QTreeWidgetItem;                       // 로그를 찍기 위한 트리위젯아이템
    items->setText(0,ip);                                               // 강퇴당한 클라이언트의 ip
    items->setText(1,QString::number(port));                            // 강퇴당한 클라이언트의 port 번호
    items->setText(2,id);                                               // 강퇴당한 클라이언트 id
    items->setText(3,name);                                             // 강퇴당한 클라이언트 이름
    items->setText(4,"ID : " + id + " Name : " + name + " Exclusion from Server");      // 강퇴당한 클라이언트 정보 메시지
    items->setToolTip(4,"ID : " + id + " Name : " + name + " Exclusion from Server");   // 메시지가 길 경우 툴팁으로 표시
    items->setText(5,QDateTime::currentDateTime().toString());          // 로그를 찍는 시간
    ui->logTreeWidget->addTopLevelItem(items);                          // 로그에 추가

    logThread->appendData(items);                                       // 로그 쓰레드에 로그 추가

    clientNameHash.remove(port);                                        // 채팅 접속 리스트에서 삭제
    sendClientList();                                                   // 채팅에 접속한 클라이언트 리스트 업데이트
}

void ChatServer::on_enteredTreeWidget_customContextMenuRequested(const QPoint &pos)     // 채팅창에서 컨텍스트 메뉴
{
    QPoint globalPos = ui->enteredTreeWidget->mapToGlobal(pos);                         // 트리위젯 어디서든 찍기 가능
    expulsionMenu->exec(globalPos);                                                     // 찍는곳에서 메뉴 표시
}


void ChatServer::on_loginClientTreeWidget_customContextMenuRequested(const QPoint &pos) // 서버 접속 창에서 컨텍스트 메뉴
{
    QPoint globalPos = ui->loginClientTreeWidget->mapToGlobal(pos);                     // 트리위젯 어디서든 찍기 가능
    admissionMenu->exec(globalPos);                                                     // 찍는곳에서 메뉴 표시
}

void ChatServer::clientAdmission()                                                      // 클라이언트 초대
{
    QByteArray sendArray;                                                               // 보낼 데이터를 담을 바이트어레이
    QDataStream out(&sendArray, QIODevice::WriteOnly);                                  // 바이트어레이를 쓰기 전용으로 열기
    out << Chat_Admission;                                                              // 프로토콜 타입 담기
    out.writeRawData("", 1020);                                                         // 타입만 보내고 데이터는 보내지 않는다

    QString id = ui->loginClientTreeWidget->currentItem()->text(0);                     // 로그를 찍기 위한 클라이언트 id
    QString name = ui->loginClientTreeWidget->currentItem()->text(1);                   // 로그를 찍기 위한 클라이언트 이름
    QTcpSocket* sock = clientSocketHash[name];                                          // 클라이언트 이름을 이용해서 소켓 특정
    quint16 port = sock->peerPort();                                                    // 초대된 클라이언트의 port 번호
    QString ip = sock->peerAddress().toString();                                        // 로그를 찍기 위한 클라이언트 ip
    sock->write(sendArray);                                                             // 소켓을 통해 데이터 전송

    QTreeWidgetItem *enterItem = new QTreeWidgetItem;                                   // 채팅창 클라이언트를 위한 트리위젯아이템
    enterItem->setText(0,id);                                                           // 초대된 클라이언트의 id
    enterItem->setText(1,clientName);                                                   // 초대된 클라이언트의 이름
    ui->enteredTreeWidget->addTopLevelItem(enterItem);                                  // 채팅창에 클라이언트 리스트 추가

    QTreeWidgetItem *items = new QTreeWidgetItem;                                       // 로그를 찍기 위한 트리위젯아이템
    items->setText(0,ip);                                                               // 초대된 클라이언트의 ip
    items->setText(1,QString::number(port));                                            // 초대된 클라이언트의 port 번호
    items->setText(2,id);                                                               // 초대된 클라이언트 id
    items->setText(3,name);                                                             // 초대된 클라이언트 이름
    items->setText(4,"ID : " + id + " Name : " + name + " Admissioned from Serverr");   // 초대된 클라이언트 정보 메시지
    items->setToolTip(4,"ID : " + id + " Name : " + name + " Admissioned from Server"); // 메시지가 길 경우 툴팁으로 표시
    items->setText(5,QDateTime::currentDateTime().toString());                          // 로그를 찍는 시간
    logThread->appendData(items);                                                       // 로그 쓰레드에 로그 추가
    clientNameHash[port] = name;                                                        // 채팅 서버 접속 리스트에 추가
    chatInClientList.append(sock);                                                      // 채팅 접속 리스트에 추가
    sendClientList();                                                                   // 채팅에 접속한 클라이언트 리스트 업데이트
}

void ChatServer::acceptConnection()                                                     // 서버에서 클라이언트에게 서버 접속 허용
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();                   // 소켓을 만들어 놓고 데이터가 올때마다 연결
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));             // 클라이언트에서 데이터를 보내면 읽는다
}

void ChatServer::readClient()                                                           // 파일 서버에서 보낸 데이터 읽기
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));                 // 파일 서버에서 보낸 소켓 특정
    QString filename, name;                                                             // 파일 이름과 보낸 클라이언트 이름
    QString client_id;                                                                  // 보낸 클라이언트 id
    if (byteReceived == 0) {                                    // 파일 전송 시작 : 파일에 대한 정보를 이용해서 QFile 객체 생성
        progressDialog->reset();                                // 프로그레스 다이얼로그 재시작
        progressDialog->show();                                 // 프로그레스 다이얼로그 표시
        QString ip = receivedSocket->peerAddress().toString();  // 데이터를 보낸 소켓의 ip
        quint16 port = receivedSocket->peerPort();              // 데이터를 보낸 소켓의 port 번호
        qDebug() << ip << " : " << port;                        // ip, port 표시
        QDataStream in(receivedSocket);                         // 데이터스트림으로 데이터 받기
        in >> totalSize >> byteReceived >> filename >> name >> client_id;   // 파일의 전체 크기를 알기 위해 데이터스트림 미리 받기
        progressDialog->setMaximum(totalSize);                  // 다 받았으면 다이얼로그 100

        QFileInfo info(filename);                               // 받은 파일의 Info
        QString currentFileName = info.fileName();              // 파일의 이름과 확장자명만 추출
        file = new QFile(currentFileName);                      // 클라이언트가 보낸 파일의 이름과 똑같은 이름으 파일을 생성
        file->open(QFile::WriteOnly);                           // 파일을 쓰기 전용으로 열기

        QTreeWidgetItem *item = new QTreeWidgetItem;            // 로그를 찍기 위한 트리위젯아이템
        item->setText(0,ip);                                    // 파일을 보낸 클라이언트의 ip
        item->setText(1,QString::number(port));                 // 파일을 보낸 클라이언트의 port 번호
        item->setText(2,client_id);                             // 파일을 보낸 클라이언트의 id
        item->setText(3,name);                                  // 파일을 보낸 클라이언트의 이름
        item->setText(4,currentFileName);                       // 보낸 파일의 이름
        item->setText(5,QDateTime::currentDateTime().toString());   // 로그를 찍는 시간
        ui->logTreeWidget->addTopLevelItem(item);               // 트리위젯에 로그 표시
        logThread->appendData(item);                            // 로그 쓰레드에 로그 추가
    } else {                                                    // 파일 데이터를 읽어서 저장
        inBlock = receivedSocket->readAll();                    // 소켓의 정보를 전부 읽기

        byteReceived += inBlock.size();                         // 받은 데이터의 바이트에 전송 받는 데이터의 크기 더하기
        file->write(inBlock);                                   // 보낼 데이터를 소켓에 담아 보내기
        file->flush();                                          // 소켓을 보내고 난 후 초기화
    }

    progressDialog->setValue(byteReceived);                     // 보낼 파일의 크기만큼 다이얼로그 설정

    if (byteReceived == totalSize) {                            // 파일의 다 읽으면 QFile 객체를 닫고 삭제
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();                                        // 보낸 바이트어레이 초기화
        byteReceived = 0;                                       // 받는 파일의 크기는 모르니까 0으로 초기화
        totalSize = 0;                                          // 전체 파일의 크기를 모르기때문에 0으로 초기화
        progressDialog->reset();                                // 파일을 다 받았으면 다이얼로그 리셋
        progressDialog->hide();                                 // 다이얼로그 숨기기

        file->close();                                          // 다 보낸 파일은 닫기
        delete file;                                            // 전송 끝난 파일 객체 삭제
    }
}

void ChatServer::removeClient()                                             // 클라이언트가 서버를 나갈때 함수
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));   // 서버를 나간 클라이언트 소켓 특정
    quint16 port = clientConnection->peerPort();                            // 서버를 나간 클라이언트 port 번호
    QString ip = clientConnection->peerAddress().toString();                // 서버를 나간 클라이언트 ip
    int id = clientIdHash[clientConnection];                                // 서버를 나간 클라이언트 id
    if(clientConnection != nullptr) {                                       // 서버를 나간 클라이언트 소켓이 사라지지 않았다면
        /* 클라이언트 id를 이용하여 */
        foreach(auto item, ui->loginClientTreeWidget->findItems(QString::number(id), Qt::MatchFixedString, 0)) {
            /* 서버에 접속한 클라이언트 리스트에서 삭제 */
            ui->loginClientTreeWidget->takeTopLevelItem(ui->loginClientTreeWidget->indexOfTopLevelItem(item));

            clientSocketHash.remove(clientName);                            // 서버에 들어와있는 클라이언트 리스트 업데이트

            QTreeWidgetItem *items = new QTreeWidgetItem;                   // 로그 표시를 위한 트리위젯 아이템
            items->setText(0,ip);                                           // 서버를 나간 클라이언트 ip
            items->setText(1,QString::number(port));                        // 서버를 나간 클라이언트 port 번호
            items->setText(2,clientId);                                     // 서버를 나간 클라이언트 id
            items->setText(3,clientName);                                   // 서버를 나간 클라이언트 이름
            items->setText(4,"ID : " + clientId + " Name : " +clientName\
                           + " Server Out");                                // 서버를 나간 소켓의 데이터를 로그에 찍기
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                              + " Server Out");                             // 로그 메시지가 길 경우 툴팁으로 표시
            items->setText(5,QDateTime::currentDateTime().toString());      // 로그를 찍는 시간
            ui->logTreeWidget->addTopLevelItem(items);                      // 트리위젯에 로그 표시
            logThread->appendData(items);                                   // 로그 쓰레드에 로그 추가
        }
        /* 클라이언트 id를 이용하여 */
        foreach(auto item, ui->enteredTreeWidget->findItems(QString::number(id), Qt::MatchFixedString, 0)) {
            /* 서버에 접속한 클라이언트 리스트 업데이트 */
            ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
        }
        clientConnection->deleteLater();                                    // 소켓이 더 이상 쓰이지 않으면 삭제
    }
}

void ChatServer::sendClientList()                                           // 채팅창에 접속한 클라이언트 리스트 업데이트
{
    foreach(QTcpSocket *sock, chatInClientList) {                           // 서버에 접속한 클라이언트 소켓을 전부 찾는다
        outByteArray.clear();                                               // 보낼 바이트어레이 초기화
        foreach(auto item, ui->enteredTreeWidget->findItems("",Qt::MatchContains)) { // 채팅에 접속한 전체 리스트
            QString name = item->text(1);                                   // 클라이언트 이름 추출
            /* 클라이언트 이름을 이용하여 */
            if(ui->enteredTreeWidget->findItems(name,Qt::MatchContains).count())
                enterClientList.append(name + "/");                         // 현재 채팅창에 들어온 클라이언트를 리스트에 저장
            outByteArray.append(name.toUtf8() + "/");                       // 클라이언트 이름을 바이트어레이에 저장
        }
        QByteArray sendArray;                                               // 소켓으로 보낼 바이트어레이
        QDataStream out(&sendArray, QIODevice::WriteOnly);                  // 데이터스트림에 소켓 저장
        out.device()->seek(0);                                              // 데이터스트림의 커서를 제일 앞 쪽으로 이동
        out << Send_Client;                                                 // 프로토콜 타입 담기
        out.writeRawData(outByteArray, 1020);                               // 프로토콜만 보내고 데이터는 보내지 않는다
        sock->write(sendArray);                                             // 소켓을 통해 데이터 전송
        sock->flush();                                                      // 데이터 전송을 마친 소켓 초기화
        while(sock->waitForBytesWritten());                                 // 다음 보낼 데이터가 올 때 까지 대기
    }
}

void ChatServer::interactiveChat()                                          // 서버와 클라이언트 간의 1:1 채팅
{
    int c_id = ui->loginClientTreeWidget->currentItem()->text(0).toInt();   // 1:1 채팅을 하는 클라이언트 id
    QString c_name = ui->loginClientTreeWidget->currentItem()->text(1);     // 1:1 채팅을 하는 클라이언트 이름
    ServerClientChat *w = new ServerClientChat(c_id,c_name);                // 1:1 채팅창을 띄울때 클라이언트 id, 이름 이용
    clientIdWindowHash[c_id] = w;                                           // 클라이언트 id와 1:1 채팅창을 해시로 관리

    /* 1:1 채팅창에서 보낸 메시지를 서버로 전송 */
    connect(clientIdWindowHash[c_id],SIGNAL(sendData(int,QString)),this,SLOT(serverToClient(int,QString)));

    w->setWindowTitle(QString::number(c_id) + " " + c_name);                // 1:1 채팅창의 이름을 클라이언트 id와 이름을 이용
    w->show();                                                              // 1:1 채팅창 보여주기
}

void ChatServer::serverToClient(int c_id, QString str)                      // 1:1 채팅으로 관리자 메시지를 클라이언트에게 전송
{
    /* 클라이언트 id를 이용하여 */
    foreach(auto item, ui->enteredTreeWidget->findItems(QString::number(c_id), Qt::MatchFixedString, 0))
    {
        QString name = item->text(1);                                       // 보낼 클라이언트 이름 추출
        QTcpSocket* sock = clientSocketHash[name];                          // 1:1 채팅을 하는 소켓 특정
        QByteArray sendArray;                                               // 보낼 데이터를 담을 바이트어레이
        sendArray.clear();                                                  // 바이트어레이 초기화
        QDataStream out(&sendArray, QIODevice::WriteOnly);                  // 데이터스트림에 보낼 바이트어레이 쓰기 전용으로 열기
        out << Chat_One;                                                    // 보낼 프로토콜 담기
        sendArray.append(str.toStdString().data());                         // 보낼 데이터 담기
        sock->write(sendArray);                                             // 소켓을 통한 데이터 전송
    }
}
