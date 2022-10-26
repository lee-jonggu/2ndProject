#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "clientmanager.h"
#include "itemmanager.h"
#include "ordermanager.h"
#include "chatserver.h"
#include "chatwindow.h"
#include "serverclientchat.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientManager = new ClientManager(this);
    ui->tabWidget->addTab(clientManager,"&Client");

    itemManager = new ItemManager(this);
    ui->tabWidget->addTab(itemManager,"&Item");
    itemManager->loadData();

    orderManager = new OrderManager(this);
    ui->tabWidget->addTab(orderManager, "&Order");
    orderManager->loadData();

    chatManager = new ChatServer(this);
    ui->tabWidget->addTab(chatManager,"&Chat Server");

//    serverClientChat = new ServerClientChat(this);

    connect(clientManager,SIGNAL(clientAdded(int,QString)),chatManager,SLOT(showIdName(int,QString)));
    connect(clientManager,SIGNAL(clientRemove(int,int)),chatManager,SLOT(removeIdName(int,int)));

    clientManager->loadData();

    //clientManager 에서 검색한 리스트를 OrderManager에 전달
    connect(clientManager, SIGNAL(clientDataSent(Client*)), orderManager, SLOT(showClientData(Client*)));
    // Qstring 받아와서 clientManager에 전해줘서 리스트를 검색
    connect(orderManager,SIGNAL(clientDataSent(int)),clientManager,SLOT(clientIdListData(int)));
    connect(orderManager,SIGNAL(clientDataSent(QString)),clientManager,SLOT(clientNameListData(QString)));
    connect(orderManager,SIGNAL(clientDataSent(QString)),clientManager,SLOT(clientAddressListData(QString)));
    connect(orderManager,SIGNAL(clientDataSent(QString)),clientManager,SLOT(clientTypeListData(QString)));

    //itemManager 에서 검색한 리스트를 OrderManager에 전달
    connect(itemManager, SIGNAL(itemDataSent(Item*)), orderManager, SLOT(showItemData(Item*)));
    // OrderManager에서 받아와서 itemManager에 전달
    connect(orderManager,SIGNAL(itemDataSent(int)),itemManager,SLOT(itemIdListData(int)));
    connect(orderManager,SIGNAL(itemDataSent(QString)),itemManager,SLOT(itemNameListData(QString)));
    connect(orderManager,SIGNAL(itemDataSent(QString)),itemManager,SLOT(itemColorListData(QString)));
    connect(orderManager,SIGNAL(itemDataSent(QString)),itemManager,SLOT(itemCategoriListData(QString)));

    // 주문창에서 고객ID, 아이템ID를 이용해서 등록하면 오더리스트에 고객 이름, 아이템 이름으로 나오게 하는 커넥트
    connect(orderManager,SIGNAL(clientNameDataSent(int,QTreeWidgetItem*)),clientManager,SLOT(clientIdNameListData(int,QTreeWidgetItem*)));
    connect(clientManager,SIGNAL(clientNameDataSent(Client*,QTreeWidgetItem*)),orderManager,SLOT(showClientNameData(Client*,QTreeWidgetItem*)));
    connect(orderManager,SIGNAL(itemNameDataSent(int,QTreeWidgetItem*)),itemManager,SLOT(itemIdNameListData(int,QTreeWidgetItem*)));
    connect(itemManager,SIGNAL(itemNameDataSent(Item*,QTreeWidgetItem*)),orderManager,SLOT(showItemNameData(Item*,QTreeWidgetItem*)));

//    connect(chatManager,SIGNAL(serverToChat(int,QString)),serverClientChat,SLOT(clientIdToName(int,QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    chatWindow = new ChatWindow(nullptr);
    chatWindow->show();
}

