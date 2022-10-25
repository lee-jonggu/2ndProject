#include "chatserver.h"
#include "logthread.h"
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
    ui->setupUi(this);

    // 채팅을 위한 서버
    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()),SLOT(clientConnect()));
    if (!tcpServer->listen(QHostAddress::Any, PORT_NUMBER))
    {
        QMessageBox::critical(this, tr("Echo Server"),tr("Unable to start the server : %1.").arg(tcpServer->errorString()));
        close();
        return;
    }

    // 파일 전송을 위한 서버
    fileServer = new QTcpServer(this);
    connect(fileServer, SIGNAL(newConnection()), SLOT(acceptConnection()));
    if (!fileServer->listen(QHostAddress::Any, PORT_NUMBER+1)) {
        QMessageBox::critical(this, tr("Chatting Server"), \
                              tr("Unable to start the server: %1.") \
                              .arg(fileServer->errorString( )));
        close( );
        return;
    }

//    infoLabel->setText(tr("The server is running on port %1.").arg(tcpServer->serverPort()));

    QAction* expulsionAction = new QAction(tr("&Expulsion"));
    connect(expulsionAction, SIGNAL(triggered()), SLOT(clientExpulsion()));
    expulsionMenu = new QMenu;
    expulsionMenu->addAction(expulsionAction);
    ui->enteredTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QAction* admissionAction = new QAction(tr("&Addmission"));
    connect(admissionAction, SIGNAL(triggered()),SLOT(clientAdmission()));
    admissionMenu = new QMenu;
    admissionMenu->addAction(admissionAction);
    ui->loginClientTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();

    logThread = new LogThread(this);
    logThread->start();

    connect(ui->logPushButton, SIGNAL(clicked()), logThread, SLOT(saveData()));
    qDebug() << "Saved the log file.";
}

ChatServer::~ChatServer()
{
    delete ui;

    logThread->terminate();
    tcpServer->close( );
    fileServer->close( );
}

void ChatServer::clientConnect()
{
    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()), SLOT(removeClient()));
    connect(clientConnection, SIGNAL(readyRead()),this,SLOT(receiveData()));

//    clientList.append(clientConnection);
}

void ChatServer::receiveData()
{

    QTcpSocket *clientConnection = (QTcpSocket *)sender();

    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    chatProtocolType data;                                          // 프로토콜타입 유형

    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    QString ip = clientConnection->peerAddress().toString();
    quint16 port = clientConnection->peerPort();
    QString id = QString::fromStdString(data.data);



    foreach(auto item, ui->totalClientTreeWidget->findItems(id,Qt::MatchFixedString,0))
    {
        clientName = item->text(1);
    }

    switch(data.type) {
    case Server_In:
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
//            QString id = item->text(0);
//            QString name = item->text(1);
            enterItem->setText(0,id);
            enterItem->setText(1,clientName);
            ui->loginClientTreeWidget->addTopLevelItem(enterItem);
            clientList.append(clientConnection);        // QList<QTcpSocket*> clientList;
            clientSocketHash[clientName] = clientConnection;
            clientNameHash[port] = clientName;
            clientIdHash[clientConnection] = id.toInt();
            clientId = id;

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,QString::number(clientIdHash[clientConnection]));
            items->setText(3,clientNameHash[port]);
//            items->setText(4,"ID : " + QString::number(clientIdHash[clientNameHash[port]]) + " Name : " + clientNameHash[port]\
//                    + " LogIn");
//            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientNameHash[port]]) + " Name : " + clientNameHash[port]\
//                    + " LogIn");
            items->setText(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");
            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
                    + " Server In");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
            logThread->appendData(item);
        }
        break;
    case Chat_In: // 케이스가 끝날때 sendprotocol ???
    {
        foreach(auto item, ui->totalClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            QTreeWidgetItem *enterItem = new QTreeWidgetItem;
            QString id = item->text(0);
            QString name = item->text(1);
            enterItem->setText(0,id);
            enterItem->setText(1,clientName);
            ui->enteredTreeWidget->addTopLevelItem(enterItem);
            clientNameHash[port] = name;
            chatInClientList.append(clientConnection);
//            chatInClient.append(name);

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting In");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
            logThread->appendData(item);

        }
        sendClientList();
    }

        break;
    case Chat_Talk: {
        foreach(QTcpSocket *sock, clientList) {
            if(clientNameHash.contains(sock->peerPort()) && sock != clientConnection) {
                QByteArray sendArray;
                sendArray.clear();
                QDataStream out(&sendArray, QIODevice::WriteOnly);
                out << Chat_Talk;
                sendArray.append(clientNameHash[port].toStdString().data());
                sendArray.append(" : ");
                sendArray.append(id.toStdString().data());
////                sendArray.append(id.toStdString());
//                outByteArray.clear();
//                outByteArray.append(id.toStdString().data());
//                out.writeRawData(outByteArray, 1020);
                sock->write(sendArray);
            }
        }

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,ip);
        item->setText(1,QString::number(port));
        item->setText(2,clientId);
        item->setText(3,clientNameHash[port]);
        item->setText(4,QString(data.data));
        item->setToolTip(4,QString(data.data));
        item->setText(5,QDateTime::currentDateTime().toString());
        ui->logTreeWidget->addTopLevelItem(item);

//        for(int i = 0; i < ui->logTreeWidget->columnCount(); i++)
//            ui->logTreeWidget->resizeColumnToContents(i);
        logThread->appendData(item);

    }
        break;
    case Chat_Out:
        qDebug() << chatInClientList;
        foreach(auto item, ui->enteredTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
            clientNameHash.remove(port);

            if(!chatInClientList.isEmpty()){
                QList<QTcpSocket*>::Iterator eraseSock;
                for(auto sock = chatInClientList.begin(); chatInClientList.end() != sock; sock++){
                    if(*sock == clientConnection){
                        eraseSock = sock;
                    }
                }
                chatInClientList.erase(eraseSock);
            }
            qDebug() << chatInClientList;

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
//            items->setText(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
//                    + " Chatting Out");
//            items->setToolTip(4,"ID : " + QString::number(clientIdHash[clientConnection]) + " Name : " + clientNameHash[port]\
//                    + " Chatting Out");
            items->setText(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Chatting Out");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
            logThread->appendData(item);
        }
        sendClientList();
        break;

    case Server_Out:
        foreach(auto item, ui->loginClientTreeWidget->findItems(id, Qt::MatchFixedString, 0))
        {
            ui->loginClientTreeWidget->takeTopLevelItem(ui->loginClientTreeWidget->indexOfTopLevelItem(item));
            clientSocketHash.remove(clientName);

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
            items->setText(4,"ID : " + clientId + " Name : " +clientName\
                    + " Server Out");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                    + " Server Out");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
            logThread->appendData(item);

        }
        sendClientList();
        break;
    }

}

void ChatServer::showIdName(int id,QString name)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0,QString::number(id));
    item->setText(1,name);
    ui->totalClientTreeWidget->addTopLevelItem(item);
//    clientIdHash[name] = id;
}

void ChatServer::removeIdName(int id,int index)
{
    Q_UNUSED(id);
    ui->totalClientTreeWidget->takeTopLevelItem(index);
}

void ChatServer::showServerClient(QTreeWidgetItem* item)
{
    ui->totalClientTreeWidget->addTopLevelItem(item);
}

void ChatServer::clientExpulsion()                          // 클라이언트 강퇴
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Expulsion;
    out.writeRawData("", 1020);

    QString id = ui->enteredTreeWidget->currentItem()->text(0);
    QString name = ui->enteredTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientSocketHash[name];
    quint16 port = sock->peerPort();
    QString ip = sock->peerAddress().toString();
    sock->write(sendArray);
    foreach(auto item, ui->enteredTreeWidget->findItems(id, Qt::MatchFixedString, 0))
    {
        ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
    }
    QTreeWidgetItem *items = new QTreeWidgetItem;
    items->setText(0,ip);
    items->setText(1,QString::number(port));
    items->setText(2,id);
    items->setText(3,name);
    items->setText(4,"ID : " + id + " Name : " + name\
            + " Exclusion from Server");
    items->setToolTip(4,"ID : " + id + " Name : " + name\
            + " Exclusion from Server");
    items->setText(5,QDateTime::currentDateTime().toString());
    ui->logTreeWidget->addTopLevelItem(items);

    logThread->appendData(items);


    clientNameHash.remove(port); // 채팅 서버 접속 리스트에서 삭제
    sendClientList();
//    chatInClient.remove(name);
}

void ChatServer::on_enteredTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->enteredTreeWidget->mapToGlobal(pos);
    expulsionMenu->exec(globalPos);
}


void ChatServer::on_loginClientTreeWidget_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->loginClientTreeWidget->mapToGlobal(pos);
    admissionMenu->exec(globalPos);
}

void ChatServer::clientAdmission()                          // 클라이언트 초대
{
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << Chat_Admission;
    out.writeRawData("", 1020);

    QString id = ui->loginClientTreeWidget->currentItem()->text(0);
    QString name = ui->loginClientTreeWidget->currentItem()->text(1);
    QTcpSocket* sock = clientSocketHash[name];
    quint16 port = sock->peerPort();
    QString ip = sock->peerAddress().toString();
    sock->write(sendArray);

    QTreeWidgetItem *enterItem = new QTreeWidgetItem;
    enterItem->setText(0,id);
    enterItem->setText(1,clientName);
    ui->enteredTreeWidget->addTopLevelItem(enterItem);

    QTreeWidgetItem *items = new QTreeWidgetItem;
    items->setText(0,ip);
    items->setText(1,QString::number(port));
    items->setText(2,id);
    items->setText(3,name);
    items->setText(4,"ID : " + id + " Name : " + name\
            + " Admissioned from Serverr");
    items->setToolTip(4,"ID : " + id + " Name : " + name\
            + " Admissioned from Server");
    items->setText(5,QDateTime::currentDateTime().toString());

    logThread->appendData(items);

    clientNameHash[port] = name; // 채팅 서버 접속 리스트에 추가
    chatInClientList.append(sock);
    sendClientList();
}

void ChatServer::acceptConnection()
{
    qDebug("Connected, preparing to receive files!");

    QTcpSocket* receivedSocket = fileServer->nextPendingConnection();
    connect(receivedSocket, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void ChatServer::readClient()
{
    qDebug("Receiving file ...");
    QTcpSocket* receivedSocket = dynamic_cast<QTcpSocket *>(sender( ));
    QString filename, name;
    QString client_id;
    if (byteReceived == 0) {        // 파일 전송 시작 : 파일에 대한 정보를 이용해서 QFile 객체 생성
        progressDialog->reset();
        progressDialog->show();
        QString ip = receivedSocket->peerAddress().toString();
        quint16 port = receivedSocket->peerPort();
        qDebug() << ip << " : " << port;
        QDataStream in(receivedSocket);
        in >> totalSize >> byteReceived >> filename >> name >> client_id;
        progressDialog->setMaximum(totalSize);


//        logThread->appendData(item);

        QFileInfo info(filename);
        QString currentFileName = info.fileName();
        file = new QFile(currentFileName);
        file->open(QFile::WriteOnly);

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,ip);
        item->setText(1,QString::number(port));
        item->setText(2,client_id);
        item->setText(3,name);
        item->setText(4,currentFileName);
        item->setText(5,QDateTime::currentDateTime().toString());
        ui->logTreeWidget->addTopLevelItem(item);
        logThread->appendData(item);
    } else {                    // 파일 데이터를 읽어서 저장
        inBlock = receivedSocket->readAll();

        byteReceived += inBlock.size();
        file->write(inBlock);
        file->flush();
    }

    progressDialog->setValue(byteReceived);

    if (byteReceived == totalSize) {        /* 파일의 다 읽으면 QFile 객체를 닫고 삭제 */
        qDebug() << QString("%1 receive completed").arg(filename);

        inBlock.clear();
        byteReceived = 0;
        totalSize = 0;
        progressDialog->reset();
        progressDialog->hide();

        file->close();
        delete file;
    }
}

void ChatServer::removeClient()
{
    QTcpSocket *clientConnection = dynamic_cast<QTcpSocket *>(sender( ));
    quint16 port = clientConnection->peerPort();
    QString ip = clientConnection->peerAddress().toString();
    int id = clientIdHash[clientConnection];
    if(clientConnection != nullptr) {
        foreach(auto item, ui->loginClientTreeWidget->findItems(QString::number(id), Qt::MatchFixedString, 0)) {
            ui->loginClientTreeWidget->takeTopLevelItem(ui->loginClientTreeWidget->indexOfTopLevelItem(item));

            clientSocketHash.remove(clientName);

            QTreeWidgetItem *items = new QTreeWidgetItem;
            items->setText(0,ip);
            items->setText(1,QString::number(port));
            items->setText(2,clientId);
            items->setText(3,clientName);
            items->setText(4,"ID : " + clientId + " Name : " +clientName\
                           + " Server Out");
            items->setToolTip(4,"ID : " + clientId + " Name : " + clientName\
                              + " Server Out");
            items->setText(5,QDateTime::currentDateTime().toString());
            ui->logTreeWidget->addTopLevelItem(items);
            logThread->appendData(items);
        }
        foreach(auto item, ui->enteredTreeWidget->findItems(QString::number(id), Qt::MatchFixedString, 0)) {
            ui->enteredTreeWidget->takeTopLevelItem(ui->enteredTreeWidget->indexOfTopLevelItem(item));
        }
        clientConnection->deleteLater();
    }
}

void ChatServer::sendClientList()
{
#if 0
    foreach(QTcpSocket *sock, clientList) { // 서버가 아니라 채팅창에 접속한 애들의 소켓으로만 한정 지어야한다
        outByteArray.clear();
        foreach(auto item, ui->enteredTreeWidget->findItems("",Qt::MatchContains)) { // 채팅에 접속한 전체 리스트
            QString name = item->text(1);
            if(ui->enteredTreeWidget->findItems(name,Qt::MatchContains).count())// 이름만 추출
                enterClientList.append(name + "/");                                           // 리스트에 저장
            //            QByteArray outByteArray;
            outByteArray.append(name.toUtf8() + "/");
        }
        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out.device()->seek(0);
        out << Send_Client;
        out.writeRawData(outByteArray, 1020);
        sock->write(sendArray);
        sock->flush();
        while(sock->waitForBytesWritten());
    }
#else
//    foreach(QTcpSocket *sock, clientList) { // 서버가 아니라 채팅창에 접속한 애들의 소켓으로만 한정 지어야한다
    foreach(QTcpSocket *sock, chatInClientList) {
        outByteArray.clear();
        foreach(auto item, ui->enteredTreeWidget->findItems("",Qt::MatchContains)) { // 채팅에 접속한 전체 리스트
            QString name = item->text(1);
            if(ui->enteredTreeWidget->findItems(name,Qt::MatchContains).count())// 이름만 추출
                enterClientList.append(name + "/");                                           // 리스트에 저장
            //            QByteArray outByteArray;
            outByteArray.append(name.toUtf8() + "/");
        }
        QByteArray sendArray;
        QDataStream out(&sendArray, QIODevice::WriteOnly);
        out.device()->seek(0);
        out << Send_Client;
        out.writeRawData(outByteArray, 1020);
        sock->write(sendArray);
        sock->flush();
        while(sock->waitForBytesWritten());
    }
#endif
}


