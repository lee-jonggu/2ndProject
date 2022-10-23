#include "clientmanager.h"
#include "client.h"
#include "ui_clientmanager.h"

#include <QFile>
#include <QMenu>
#include <time.h>
#include <QTime>
#include <QDebug>

ClientManager::ClientManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientManager)
{
    ui->setupUi(this);

    QList<int> sizes;
    sizes << 300 << 660;
    ui->splitter->setSizes(sizes);

    connect(ui->ClientInfoTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showClient(QTreeWidgetItem*,int)));
}

void ClientManager::loadData()
{
    QFile file("clientlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            Client* c = new Client(id, row[1], row[2], row[3], row[4]);
            ui->ClientInfoTreeWidget->addTopLevelItem(c);
            clientList.insert(id, c);

            emit clientAdded(id,row[1]);
        }
    }
    file.close( );
}

ClientManager::~ClientManager()
{
    delete ui;

    QFile file("clientlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : clientList) {
        Client* c = v;
        out << c->id() << ", " << c->getName() << ", ";
        out << c->getAddress() << ", ";
        out << c->getPhoneNum() << ", ";
        out << c->getType() << "\n";
    }
    file.close( );

}

void ClientManager::on_ClientInfoAddPushButton_clicked()
{
    QString name, address, phoneNum, type;
    int id = makeId( );
    name = ui->ClientInfoInputNameLineEdit->text();
    address = ui->ClientInfoInputAddressLineEdit->text();
    phoneNum = ui->ClientInfoInputPhoneNumLineEdit->text();
    type = ui->ClientInfoInputTypeLineEdit->text();
    if(name.length()) {
        Client* c = new Client(id, name, address, phoneNum, type);
        clientList.insert(id, c);
        ui->ClientInfoTreeWidget->addTopLevelItem(c);
        emit clientAdded(id,name);
    }

    addLogList.insert(id,QTime::currentTime().toString());

    ui->ClientInfoTimeListWidget->clear();

    ui->ClientInfoInputNameLineEdit->clear();
    ui->ClientInfoInputAddressLineEdit->clear();
    ui->ClientInfoInputPhoneNumLineEdit->clear();
    ui->ClientInfoInputTypeLineEdit->clear();

    ui->ClientInfoTimeListWidget->addItem("Added Time : "+QTime::currentTime().toString());
}


void ClientManager::on_ClientInfoModifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->ClientInfoTreeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        Client* c = clientList[key];
        QString name, address, phoneNum, type;
        name = ui->ClientInfoInputNameLineEdit->text();
        address = ui->ClientInfoInputAddressLineEdit->text();
        phoneNum = ui->ClientInfoInputPhoneNumLineEdit->text();
        type = ui->ClientInfoInputTypeLineEdit->text();
        c->setName(name);
        c->setAddress(address);
        c->setPhoneNum(phoneNum);
        c->setType(type);
        clientList[key] = c;

        logTimeList.insert(key,QTime::currentTime().toString());
//        ui->ClientInfoTimeListWidget->clear();
    }
}


void ClientManager::on_ClientInfoRemovePushButton_clicked()
{
    QTreeWidgetItem* item = ui->ClientInfoTreeWidget->currentItem();
    if(item != nullptr) {
        clientList.remove(item->text(0).toInt());
        int itemidx =  ui->ClientInfoTreeWidget->indexOfTopLevelItem(item);

        ui->ClientInfoTreeWidget->takeTopLevelItem(ui->ClientInfoTreeWidget->indexOfTopLevelItem(item));
        ui->ClientInfoTreeWidget->update();

        ui->ClientInfoInputNameLineEdit->clear();
        ui->ClientInfoInputAddressLineEdit->clear();
        ui->ClientInfoInputPhoneNumLineEdit->clear();
        ui->ClientInfoInputTypeLineEdit->clear();

        emit clientRemove(item->text(0).toInt(),itemidx);
    }
}

int ClientManager::makeId( )
{
    if(clientList.size( ) == 0) {
        return 1000;
    } else {
        auto id = clientList.lastKey();
        return ++id;
    }
}


void ClientManager::on_ClientSearchPushButton_clicked()
{
    ui->ClientSearchTreeWidget->clear();

    int i = ui->ClientSearchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->ClientInfoTreeWidget->findItems(ui->ClientSearchInputLineEdit->text(), flag, i);

        foreach(auto i, items) {
            Client* c = static_cast<Client*>(i);
            int id = c->id();
            QString name = c->getName();
            QString address = c->getAddress();
            QString number = c->getPhoneNum();
            QString type = c->getType();
            Client* item = new Client(id, name, address, number, type);
            ui->ClientSearchTreeWidget->addTopLevelItem(item);
        }
    }
}

void ClientManager::showClient(QTreeWidgetItem* item,int e)
{
    QTreeWidgetItem* c = ui->ClientInfoTreeWidget->currentItem();
    if(c != nullptr)
    {
        Q_UNUSED(e);
        ui->ClientInfoTimeListWidget->clear();

        ui->ClientInfoInputIdLineEdit->setText(item->text(0));
        ui->ClientInfoInputNameLineEdit->setText(item->text(1));
        ui->ClientInfoInputAddressLineEdit->setText(item->text(2));
        ui->ClientInfoInputPhoneNumLineEdit->setText(item->text(3));
        ui->ClientInfoInputTypeLineEdit->setText(item->text(4));

        int key = item->text(0).toInt();
        for (auto& v : logTimeList.values(key))
        {
            ui->ClientInfoTimeListWidget->addItem("Modified Time : "+v);
        }
        ui->ClientInfoTimeListWidget->addItem("Added Time : "+addLogList[key]);
    }
}

void ClientManager::clientIdListData(int id)
{
    auto items = ui->ClientInfoTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {
        Client* c = static_cast<Client*>(i);
        int id = c->id();
        QString name = c->getName();
        QString address = c->getAddress();
        QString number = c->getPhoneNum();
        QString type = c->getType();
        Client* item = new Client(id, name, address, number, type);
        emit clientDataSent(item);
    }
}


void ClientManager::clientNameListData(QString cstr)
{
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,1);

    foreach(auto i, items) {
        Client* c = static_cast<Client*>(i);
        int id = c->id();
        QString name = c->getName();
        QString address = c->getAddress();
        QString number = c->getPhoneNum();
        QString type = c->getType();
        Client* item = new Client(id, name, address, number, type);
        emit clientDataSent(item);
    }
}

void ClientManager::clientAddressListData(QString cstr)
{
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,2);

    foreach(auto i, items) {
        Client* c = static_cast<Client*>(i);
        int id = c->id();
        QString name = c->getName();
        QString address = c->getAddress();
        QString number = c->getPhoneNum();
        QString type = c->getType();
        Client* item = new Client(id, name, address, number, type);
        emit clientDataSent(item);
    }
}

void ClientManager::clientTypeListData(QString cstr)
{
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,3);

    foreach(auto i, items) {
        Client* c = static_cast<Client*>(i);
        int id = c->id();
        QString name = c->getName();
        QString address = c->getAddress();
        QString number = c->getPhoneNum();
        QString type = c->getType();
        Client* item = new Client(id, name, address, number, type);
        emit clientDataSent(item);
    }
}

void ClientManager::clientIdNameListData(int id,QTreeWidgetItem* row)
{
    Q_UNUSED(row);
    auto items = ui->ClientInfoTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {
        Client* c = static_cast<Client*>(i);
        QString name = c->getName();
        QString address = c->getAddress();
        QString number = c->getPhoneNum();
        QString type = c->getType();
        Client* client = new Client(id, name, address, number, type);
        emit clientNameDataSent(client,row);
    }
}

void ClientManager::serverClientList()
{
    foreach(auto i, clientList)
    {
        Client* c = static_cast<Client*>(i);
        int id = c->id();
        QString name = c->getName();

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0,QString::number(id));
        item->setText(1,name);
        serverClient.insert(id,name);
        emit clientToServer(item);
    }
}
