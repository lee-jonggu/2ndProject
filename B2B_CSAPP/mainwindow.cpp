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

    clientManager = new ClientManager(this);                    // 클라이언트 매니저 생성
    ui->tabWidget->addTab(clientManager,"&Client");             // 탭위젯에 추가

    itemManager = new ItemManager(this);                        // 아이템 매니저 생성
    ui->tabWidget->addTab(itemManager,"&Item");                 // 탭위젯에 추가
    itemManager->loadData();                                    // 저장된 데이터 불러오기

    orderManager = new OrderManager(this);                      // 오더 매니저 생성
    ui->tabWidget->addTab(orderManager, "&Order");              // 탭위젯 추가
    orderManager->loadData();                                   // 저장된 데이터 불러오기

    chatManager = new ChatServer(this);                         // 채팅 매니저 생성
    ui->tabWidget->addTab(chatManager,"&Chat Server");          // 탭위젯 추가

    /* 클라이언트매니저와 채팅매니저 연결 */
    connect(clientManager,SIGNAL(clientAdded(int,QString)),chatManager,SLOT(showIdName(int,QString)));
    connect(clientManager,SIGNAL(clientRemove(int,int)),chatManager,SLOT(removeIdName(int,int)));

    clientManager->loadData();                                  // 저장된 데이터 불러오기

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
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    chatWindow = new ChatWindow(nullptr);                       // 채팅 윈도우 생성
    chatWindow->show();                                         // 채팅 윈도우 표시
}

