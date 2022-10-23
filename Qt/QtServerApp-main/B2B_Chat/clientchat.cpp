#include "clientchat.h"
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

    ui->inputLine->setDisabled(true);

    ui->ipAddressLineEdit->setText("127.0.0.1");
    QRegularExpression re("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator validator(re);
    ui->ipAddressLineEdit->setPlaceholderText("Server IP Address");
    ui->ipAddressLineEdit->setValidator(&validator);

    ui->portLineEdit->setText("8010");
    ui->portLineEdit->setInputMask("00000;_");
    ui->portLineEdit->setPlaceholderText("Server Port No");

    ui->clientIdLineEdit->setInputMask("0000;_");
    ui->clientIdLineEdit->setPlaceholderText("ID No");
    ui->clientNameLineEdit->setPlaceholderText("Your Name");

    ui->chatPushButton->setDisabled(true);
    ui->chatOutPushButton->setDisabled(true);
    ui->disConnectPushButton->setDisabled(true);

    // 채팅을 위한 소켓
    clientSocket = new QTcpSocket(this);
    connect(clientSocket, SIGNAL(readyRead()),this,SLOT(receiveData()));
    connect(clientSocket, SIGNAL(disconnected( )), SLOT(disconnect( )));

    // 파일 전송을 위한 소켓
    fileSocket = new QTcpSocket(this);
    connect(fileSocket, SIGNAL(bytesWritten(qint64)), SLOT(goOnSend(qint64)));
    progressDialog = new QProgressDialog(0);
    progressDialog->setAutoClose(true);
    progressDialog->reset();
}

ClientChat::~ClientChat()
{
    clientSocket->close();
}

void ClientChat::disconnect()
{
    QMessageBox::critical(this, tr("Chatting Client"), tr("Disconnect from Server"));
    ui->chatPushButton->setDisabled(true);
    ui->chatOutPushButton->setDisabled(true);
    ui->disConnectPushButton->setDisabled(true);
}

void ClientChat::on_sendButton_clicked()
{
    sendData();
    ui->inputLine->clear();
}


void ClientChat::on_lineEdit_returnPressed()
{
    sendData();
    ui->inputLine->clear();
}

void ClientChat::receiveData()
{
    QTcpSocket *clientConnection = (QTcpSocket *)sender();
    if (clientConnection->bytesAvailable() > BLOCK_SIZE) return;
    QByteArray bytearray = clientConnection->read(BLOCK_SIZE);

    chatProtocolType data;                                          // 프로토콜타입 유형
    QDataStream in(&bytearray, QIODevice::ReadOnly);
    in >> data.type;
    in.readRawData(data.data, 1020);

    switch(data.type){
    case Chat_Talk:
        foreach(QTcpSocket *sock, clientList) {
            if (sock != clientConnection)
                sock->write(bytearray);
        }
        ui->textEdit->append(QString(data.data));
        break;
    case Chat_Expulsion:
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Exclusion from Server"));
        ui->inputLine->setDisabled(true);
        ui->chatOutPushButton->setDisabled(true);
        ui->chatPushButton->setDisabled(false);
        ui->disConnectPushButton->setDisabled(false);
        break;
    case Chat_Admisson:
        QMessageBox::critical(this, tr("Chatting Client"), \
                              tr("Admissioned from Server"));
        ui->inputLine->setDisabled(false);
        ui->chatPushButton->setDisabled(true);
        ui->chatOutPushButton->setDisabled(false);
        ui->disConnectPushButton->setDisabled(true);
        ui->connectPushButton->setDisabled(true);
        ui->clientIdLineEdit->setReadOnly(true);
        ui->clientNameLineEdit->setReadOnly(true);
        break;
    case Send_Client:
        qDebug() << data.data;
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,data.data);
        ui->treeWidget->addTopLevelItem(item);
        break;
    }

}

void ClientChat::sendData()
{
    QString str = ui->inputLine->text();
    if(str.length())
    {
        QByteArray bytearray;
        bytearray = str.toUtf8();
        ui->textEdit->append("나 : " + str);

        sendProtocol(Chat_Talk, bytearray.data());
    }
}


void ClientChat::on_connectPushButton_clicked()
{
    clientSocket->connectToHost(ui->ipAddressLineEdit->text(), ui->portLineEdit->text().toInt());
    connect(clientSocket, &QAbstractSocket::errorOccurred,
            [=]{ qDebug() << clientSocket->errorString();});

    chatProtocolType data;
    data.type = Server_In;
    qstrcpy(data.data, ui->clientIdLineEdit->text().toStdString().data());
    QByteArray sendArray;
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out << data.type;
    out.writeRawData(data.data, 1020);
    clientSocket->write(sendArray);

    ui->connectPushButton->setDisabled(true);
    ui->connectPushButton->setText("connected");
    ui->disConnectPushButton->setDisabled(false);

    ui->chatPushButton->setDisabled(false);

    ui->ipAddressLineEdit->setReadOnly(true);
    ui->portLineEdit->setReadOnly(true);
    ui->clientIdLineEdit->setReadOnly(true);
    ui->clientNameLineEdit->setReadOnly(true);
}

void ClientChat::sendProtocol(Chat_Status type, char* data, int size)
{
    QByteArray sendArray;           // 소켓으로 보낼 데이터를 채우고
    QDataStream out(&sendArray, QIODevice::WriteOnly);
    out.device()->seek(0);
    out << type;
    out.writeRawData(data, size);
    clientSocket->write(sendArray);     // 서버로 전송
    clientSocket->flush();
    while(clientSocket->waitForBytesWritten());
}

void ClientChat::on_chatPushButton_clicked()
{
    sendProtocol(Chat_In, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(false);
    ui->chatPushButton->setDisabled(true);
    ui->chatOutPushButton->setDisabled(false);
    ui->disConnectPushButton->setDisabled(true);
    ui->connectPushButton->setDisabled(true);
    ui->clientIdLineEdit->setReadOnly(true);
    ui->clientNameLineEdit->setReadOnly(true);
}

void ClientChat::on_chatOutPushButton_clicked()
{
    sendProtocol(Chat_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->inputLine->setDisabled(true);
    ui->chatOutPushButton->setDisabled(true);
    ui->chatPushButton->setDisabled(false);
    ui->disConnectPushButton->setDisabled(false);
}

void ClientChat::on_disConnectPushButton_clicked()
{
    sendProtocol(Server_Out, ui->clientIdLineEdit->text().toStdString().data());
    ui->disConnectPushButton->setDisabled(true);
    ui->connectPushButton->setDisabled(false);
    ui->chatPushButton->setDisabled(true);
    ui->clientIdLineEdit->setReadOnly(false);
    ui->clientNameLineEdit->setReadOnly(false);
    ui->ipAddressLineEdit->setReadOnly(false);
    ui->portLineEdit->setReadOnly(false);
}

void ClientChat::on_fileTransferPushButton_clicked()
{
    sendFile();
    ui->fileTransferPushButton->setDisabled(true);
}

void ClientChat::sendFile()
{
    loadSize = 0;
    byteToWrite = 0;
    totalSize = 0;
    outBlock.clear();

    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.length()) {
        file = new QFile(filename);
        file->open(QFile::ReadOnly);

        qDebug() << QString("file %1 is opened").arg(filename);
        progressDialog->setValue(0); // Not sent for the first time

        if (!isSent) { // Only the first time it is sent, it happens when the connection generates the signal connect
            fileSocket->connectToHost(ui->ipAddressLineEdit->text( ),
                                      ui->portLineEdit->text( ).toInt( ) + 1);
            isSent = true;
        }

        // When sending for the first time, connectToHost initiates the connect signal to call send, and you need to call send after the second time

        byteToWrite = totalSize = file->size(); // The size of the remaining data
        loadSize = 1024; // The size of data sent each time

        QDataStream out(&outBlock, QIODevice::WriteOnly);
//        out << qint64(0) << qint64(0) << filename << ui->clientNameLineEdit->text();
        out << qint64(0) << qint64(0) << filename << ui->clientNameLineEdit->text() << ui->clientIdLineEdit->text();
        totalSize += outBlock.size(); // The total size is the file size plus the size of the file name and other information
        byteToWrite += outBlock.size();

        out.device()->seek(0); // Go back to the beginning of the byte stream to write a qint64 in front, which is the total size and file name and other information size
        out << totalSize << qint64(outBlock.size());

        fileSocket->write(outBlock); // Send the read file to the socket

        progressDialog->setMaximum(totalSize);
        progressDialog->setValue(totalSize-byteToWrite);
        progressDialog->show();
    }
    qDebug() << QString("Sending file %1").arg(filename);
}

void ClientChat::goOnSend(qint64 numBytes)
{
    byteToWrite -= numBytes; // Remaining data size
    outBlock = file->read(qMin(byteToWrite, numBytes));
    fileSocket->write(outBlock);

    progressDialog->setMaximum(totalSize);
    progressDialog->setValue(totalSize-byteToWrite);

    if (byteToWrite == 0) { // Send completed
        qDebug("File sending completed!");
        progressDialog->reset();
    }
}

void ClientChat::closeEvent(QCloseEvent*)
{
    sendProtocol(Server_Out, name->text().toStdString().data());
    clientSocket->disconnectFromHost();
    if(clientSocket->state() != QAbstractSocket::UnconnectedState)
        clientSocket->waitForDisconnected();
}
