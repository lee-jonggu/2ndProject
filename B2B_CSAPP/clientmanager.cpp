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

    QList<int> sizes;                                                       // ui의 스플리터 사이즈 조절을 위한 사이즈
    sizes << 300 << 660;                                                    // 사이즈 설정
    ui->splitter->setSizes(sizes);                                          // ui 스플리터 사이즈 설정

    /* 등록된 클라이언트를 클릭하면 클라이언트 정보를 입력하는 창에 아이템 표시 */
    connect(ui->ClientInfoTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showClient(QTreeWidgetItem*,int)));
}

void ClientManager::loadData()                                              // 미리 저장된 클라이언트 데이터를 가져온다
{
    QFile file("clientlist.txt");                                           // 저장되어 있는 클라이언트 데이터 파일명
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))                  // 파일이 있다면 읽기전용으로 열기
        return;

    QTextStream in(&file);                                                  // 클라이언트 데이터가 담겨있는 파일을 텍스트스트림에 담기
    while (!in.atEnd()) {                                                   // 파일의 끝까지 가면서
        QString line = in.readLine();                                       // 한줄씩 읽는다
        QList<QString> row = line.split(", ");                              // 쉼표(,)로 구분하면서 한줄씩 파싱
        if(row.size()) {                                                    // 읽을 데이터가 남아있다면
            int id = row[0].toInt();                                        // 첫번째 파싱을 클라이언트 id
            Client* c = new Client(id, row[1], row[2], row[3], row[4]);     // 나머지 데이터를 클라이언트 형태로 저장
            ui->ClientInfoTreeWidget->addTopLevelItem(c);                   // 저장된 데이터를 정보 위젯에 올린다
            clientList.insert(id, c);                                       // 읽어온 데이터를 클라이언트 리스트에 추가

            emit clientAdded(id,row[1]);                                    // 클라이언트가 추가될 때 시그널 발생
        }
    }
    file.close( );                                                          // 파일을 다 읽으면 파일 닫기
}

ClientManager::~ClientManager()                                             // 소멸자에서 데이터 저장
{
    delete ui;

    QFile file("clientlist.txt");                                           // 텍스트파일 이름
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                 // 읽을 파일이 없다면 리턴
        return;

    QTextStream out(&file);                                                 // 텍스트스트림에 담을 파일 생성
    for (const auto& v : clientList) {                                      // 저장되어있는 클라이언트 리스트 돌면서
        Client* c = v;                                                      // 클라이언트 형태로
        out << c->id() << ", " << c->getName() << ", ";                     // 클라이언트 id, 이름
        out << c->getAddress() << ", ";                                     // 클라이언트 주소
        out << c->getPhoneNum() << ", ";                                    // 클라이언트 전화번호
        out << c->getType() << "\n";                                        // 클라이언트 타입 저장
    }
    file.close( );                                                          // 파일을 다 쓰고 파일 닫기

}

void ClientManager::on_ClientInfoAddPushButton_clicked()                    // add 버튼
{
    QString name, address, phoneNum, type;                                  // 이름, 주소, 전화번호, 타입
    int id = makeId( );                                                     // 클라이언트 id는 매번 새로 생성
    name = ui->ClientInfoInputNameLineEdit->text();                         // 라인에딧에 있는 이름 사용
    address = ui->ClientInfoInputAddressLineEdit->text();                   // 라인에딧에 있는 주소 사용
    phoneNum = ui->ClientInfoInputPhoneNumLineEdit->text();                 // 라인에딧에 있는 전화번호 사용
    type = ui->ClientInfoInputTypeLineEdit->text();                         // 라인에딧에 잇는 타입 사용
    if(name.length()) {                                                     // 클라이언트 이름이 비어있지 않으면
        Client* c = new Client(id, name, address, phoneNum, type);          // 클라이언트 형태로 객체 생성
        clientList.insert(id, c);                                           // 클라이언트 리스트에 저장
        ui->ClientInfoTreeWidget->addTopLevelItem(c);                       // 클라이언트 리스트에 출력
        emit clientAdded(id,name);                                          // 클라이언트가 추가될 때 시그널 발생
    }

    addLogList.insert(id,QTime::currentTime().toString());                  // 로그를 관리하기 위해 로그 리스트에 저장

    ui->ClientInfoTimeListWidget->clear();                                  // 리스트를 저장하고 입력창 초기화
    ui->ClientInfoInputNameLineEdit->clear();                               // 리스트를 저장하고 입력창 초기화
    ui->ClientInfoInputAddressLineEdit->clear();                            // 리스트를 저장하고 입력창 초기화
    ui->ClientInfoInputPhoneNumLineEdit->clear();                           // 리스트를 저장하고 입력창 초기화
    ui->ClientInfoInputTypeLineEdit->clear();                               // 리스트를 저장하고 입력창 초기화

    ui->ClientInfoTimeListWidget->addItem("Added Time : "+QTime::currentTime().toString()); // 로그창에 출력
}


void ClientManager::on_ClientInfoModifyPushButton_clicked()                 // modify 버튼 클릭
{
    QTreeWidgetItem* item = ui->ClientInfoTreeWidget->currentItem();        // 저장되어 있는 리스트에서 수정할 클라이언트 클릭
    if(item != nullptr) {                                                   // 클라이언트가 있다면
        int key = item->text(0).toInt();                                    // 클라이언트 id로 key 설정
        Client* c = clientList[key];                                        // 클라이언트 리스트를 클라이언트 id로 관리
        QString name, address, phoneNum, type;                              // 이름, 주소, 전화번호, 타입
        name = ui->ClientInfoInputNameLineEdit->text();                     // 수정할 이름 쓰기
        address = ui->ClientInfoInputAddressLineEdit->text();               // 수정할 주소 쓰기
        phoneNum = ui->ClientInfoInputPhoneNumLineEdit->text();             // 수정할 전화번호 쓰기
        type = ui->ClientInfoInputTypeLineEdit->text();                     // 수정할 타입 쓰기
        c->setName(name);                                                   // 새로운 이름으로 수정
        c->setAddress(address);                                             // 새로운 주소로 수정
        c->setPhoneNum(phoneNum);                                           // 새로운 전화번호로 수정
        c->setType(type);                                                   // 새로운 타입으로 수정
        clientList[key] = c;                                                // 바뀐 정보를 가지고 클라이언트 리스트 덮어쓰기

        logTimeList.insert(key,QTime::currentTime().toString());            // 바뀐 정보를 로그에 찍기
    }
}


void ClientManager::on_ClientInfoRemovePushButton_clicked()                 // remove 버튼 클릭
{
    QTreeWidgetItem* item = ui->ClientInfoTreeWidget->currentItem();        // 저장되어 있는 리스트에서 삭제할 아이템 클릭
    if(item != nullptr) {                                                   // 아이템이 있다면
        clientList.remove(item->text(0).toInt());                           // 클라이언트 리스트에서 id를 가지고 삭제
        int itemidx =  ui->ClientInfoTreeWidget->indexOfTopLevelItem(item); // 선택되어 있는 아이템의 인덱스를 반환

        ui->ClientInfoTreeWidget->takeTopLevelItem(ui->ClientInfoTreeWidget->indexOfTopLevelItem(item));    // 인덱스를 가지고 트리위젯에서 삭제
        ui->ClientInfoTreeWidget->update();                                                                 // 삭제 후 리스트 다시 보여주기

        ui->ClientInfoInputNameLineEdit->clear();                           // 이름 입력창 초기화
        ui->ClientInfoInputAddressLineEdit->clear();                        // 주소 입력창 초기화
        ui->ClientInfoInputPhoneNumLineEdit->clear();                       // 전화번호 입력창 초기화
        ui->ClientInfoInputTypeLineEdit->clear();                           // 타입 입력창 초기화

        emit clientRemove(item->text(0).toInt(),itemidx);                   // 클라이언트 삭제 할 id,인덱스 시그널 보내기
    }
}

int ClientManager::makeId( )                                                // 클라이언트 id 생성
{
    if(clientList.size( ) == 0) {                                           // 등록된 클라이언트가 없다면
        return 1000;                                                        // id 1000번 부여
    } else {                                                                // 등록된 클라이언트가 있다면
        auto id = clientList.lastKey();                                     // 클라이언트 리스트의 마지막 키값을 가지고
        return ++id;                                                        // 키값을 1 추가해서 새로운 id 생성
    }
}


void ClientManager::on_ClientSearchPushButton_clicked()                     // search 버튼 클릭
{
    ui->ClientSearchTreeWidget->clear();                                    // 조회한 트리위젯 초기화 후

    int i = ui->ClientSearchComboBox->currentIndex();                       // 검색할 id,이름,전화번호, 타입을 설정
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains               // id로 검색하면 정확한 값, 나머지는 대소문자 구분
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->ClientInfoTreeWidget->findItems(ui->ClientSearchInputLineEdit->text(), flag, i);   // 조회할 아이템을 가져온다

        foreach(auto i, items) {                                            // 아이템을 돌아가면서
            Client* c = static_cast<Client*>(i);                            // 클라이언트 형태로 만들고
            int id = c->id();                                               // 클라이언트 id 가져오기
            QString name = c->getName();                                    // 클라이언트 이름 가져오기
            QString address = c->getAddress();                              // 클라이언트 주소 가져오기
            QString number = c->getPhoneNum();                              // 클라이언트 전화번호 가져오기
            QString type = c->getType();                                    // 클라이언트 타입 가져오기
            Client* item = new Client(id, name, address, number, type);     // 가져온 정보를을 클라이언트 형태로 저장
            ui->ClientSearchTreeWidget->addTopLevelItem(item);              // 저장한 아이템을 트리위젯에 추가
        }
    }
}

void ClientManager::showClient(QTreeWidgetItem* item,int e)                 // 클라이언트 리스트에 있는 클라이언트를 클릭하면 입력창에 보인다
{
    QTreeWidgetItem* c = ui->ClientInfoTreeWidget->currentItem();           // 현재 선택한 클라이언트
    if(c != nullptr)                                                        // 가 있다면
    {
        Q_UNUSED(e);                                                        // 인덱스는 쓰지 않고
        ui->ClientInfoTimeListWidget->clear();                              // 기존 타임 리스트 삭제

        ui->ClientInfoInputIdLineEdit->setText(item->text(0));              // 선택된 클라이언트 아이디를 입력창에 출력
        ui->ClientInfoInputNameLineEdit->setText(item->text(1));            // 선택된 클라이언트 이름을 입력창에 출력
        ui->ClientInfoInputAddressLineEdit->setText(item->text(2));         // 선택된 클라이언트 주소를 입력창에 출력
        ui->ClientInfoInputPhoneNumLineEdit->setText(item->text(3));        // 선택된 클라이언트 전화번호를 입력창에 출력
        ui->ClientInfoInputTypeLineEdit->setText(item->text(4));            // 선택된 클라이언트 타입을 입력창에 출력

        int key = item->text(0).toInt();                                    // 보여준 클라이언트 id 저장
        for (auto& v : logTimeList.values(key))                             // id를 가지고 클라이언트 형태로 저장
        {
            ui->ClientInfoTimeListWidget->addItem("Modified Time : "+v);    // 로그창에 수정된 시간, 정보 출력
        }
        ui->ClientInfoTimeListWidget->addItem("Added Time : "+addLogList[key]); // 클라이언트가 처음 추가될 때 로그를 출력
    }
}

void ClientManager::clientIdListData(int id)                                // 클라이언트 id를 가지고 클라이언트 정보 추출
{
    /* 클라이언트 id를 이용하여 */
    auto items = ui->ClientInfoTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {                                                // 저장된 클라이언트 리스트를 돌면서
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        int id = c->id();                                                   // 클라이언트 id 가져오기
        QString name = c->getName();                                        // 클라이언트 이름 가져오기
        QString address = c->getAddress();                                  // 클라이언트 주소 가져오기
        QString number = c->getPhoneNum();                                  // 클라이언트 전화번호 가져오기
        QString type = c->getType();                                        // 클라이언트 타입 가져오기
        Client* item = new Client(id, name, address, number, type);         // 클라이언트 형태로 객체 저장
        emit clientDataSent(item);                                          // 저장된 객체를 시그널로 전송
    }
}


void ClientManager::clientNameListData(QString cstr)                        // 클라이언트 이름을 가지고 클라이언트 정보 추출
{
    /* 클라이언트 이름을 이용하여 */
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,1);

    foreach(auto i, items) {                                                // 저장된 클라이언트 리스트를 돌면서
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        int id = c->id();                                                   // 클라이언트 id 가져오기
        QString name = c->getName();                                        // 클라이언트 이름 가져오기
        QString address = c->getAddress();                                  // 클라이언트 주소 가져오기
        QString number = c->getPhoneNum();                                  // 클라이언트 전화번호 가져오기
        QString type = c->getType();                                        // 클라이언트 타입 가져오기
        Client* item = new Client(id, name, address, number, type);         // 클라이언트 형태로 객체 저장
        emit clientDataSent(item);                                          // 저장된 객체를 시그널로 전송
    }
}

void ClientManager::clientAddressListData(QString cstr)                     // 클라이언트 주소를 가지고 클라이언트 정보 추출
{
    /* 클라이언트 주소를 이용하여 */
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,2);

    foreach(auto i, items) {                                                // 저장된 클라이언트 리스트를 돌면서
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        int id = c->id();                                                   // 클라이언트 id 가져오기
        QString name = c->getName();                                        // 클라이언트 이름 가져오기
        QString address = c->getAddress();                                  // 클라이언트 주소 가져오기
        QString number = c->getPhoneNum();                                  // 클라이언트 전화번호 가져오기
        QString type = c->getType();                                        // 클라이언트 타입 가져오기
        Client* item = new Client(id, name, address, number, type);         // 클라이언트 형태로 객체 저장
        emit clientDataSent(item);                                          // 저장된 객체를 시그널로 전송
    }
}

void ClientManager::clientTypeListData(QString cstr)                     // 클라이언트 타입을 가지고 클라이언트 정보 추출
{
    /* 클라이언트 타입을 이용하여 */
    auto items = ui->ClientInfoTreeWidget->findItems(cstr,Qt::MatchContains,3);

    foreach(auto i, items) {                                                // 저장된 클라이언트 리스트를 돌면서
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        int id = c->id();                                                   // 클라이언트 id 가져오기
        QString name = c->getName();                                        // 클라이언트 이름 가져오기
        QString address = c->getAddress();                                  // 클라이언트 주소 가져오기
        QString number = c->getPhoneNum();                                  // 클라이언트 전화번호 가져오기
        QString type = c->getType();                                        // 클라이언트 타입 가져오기
        Client* item = new Client(id, name, address, number, type);         // 클라이언트 형태로 객체 저장
        emit clientDataSent(item);                                          // 저장된 객체를 시그널로 전송
    }
}

void ClientManager::clientIdNameListData(int id,QTreeWidgetItem* row)       // 클라이언트 id를 가지고 트리위젯아이템 형태로 추출
{
    Q_UNUSED(row);                                                          // 오더폼에서 사용하는 행
    /* 클라이언트 id를 이용하여 */
    auto items = ui->ClientInfoTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {                                                // 저장된 클라이언트 리스트를 돌면서
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        QString name = c->getName();                                        // 클라이언트 이름 가져오기
        QString address = c->getAddress();                                  // 클라이언트 주소 가져오기
        QString number = c->getPhoneNum();                                  // 클라이언트 전화번호 가져오기
        QString type = c->getType();                                        // 클라이언트 타입 가져오기
        Client* client = new Client(id, name, address, number, type);       // 클라이언트 형태로 객체 저장
        emit clientNameDataSent(client,row);                                // 저장된 객체를 시그널로 전송
    }
}

void ClientManager::serverClientList()                                      // 서버로 클라이언트 정보 전송
{
    foreach(auto i, clientList)                                             // 저장된 클라이언트 리스트를 돌면서
    {
        Client* c = static_cast<Client*>(i);                                // 클라이언트 형태로 저장
        int id = c->id();                                                   // 클라이언트 id 추출
        QString name = c->getName();                                        // 클라이언트 이름 추출

        QTreeWidgetItem *item = new QTreeWidgetItem;                        // 트리위젯아이템으로
        item->setText(0,QString::number(id));                               // 클라이언트 id 설정
        item->setText(1,name);                                              // 클라이언트 이름 설정
        serverClient.insert(id,name);                                       // 서버에 접속한 리스트 관리
        emit clientToServer(item);                                          // 저장된 객체를 시그널로 서버에 전송
    }
}
