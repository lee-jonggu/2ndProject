#include "ordermanager.h"
#include "itemmanager.h"
#include "order.h"
#include "ui_ordermanager.h"

#include <QTime>
#include <QFile>

OrderManager::OrderManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderManager)
{
    ui->setupUi(this);

    QList<int> sizes;                                                           // ui의 스플리터 사이즈 조절을 위한 사이즈
    sizes << 450 << 630;                                                        // 사이즈 설정
    ui->splitter->setSizes(sizes);                                              // ui 스플리터 사이즈 설정
}

OrderManager::~OrderManager()                                                   // 소멸자에서 데이터 저장
{

    QFile file("orderlist.txt");                                                // 텍스트파일 이름
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                     // 읽을 파일이 없다면 리턴
        return;

    QTextStream out(&file);                                                     // 텍스트스트림에 담을 파일 생성
    foreach (auto v , ui->orderTreeWidget->findItems("",Qt::MatchContains)) {   // 저장되어있는 아이템 리스트 돌면서
        QTreeWidgetItem* item = v;                                              // 오더 형태로
        out << item->text(0) << ", " << item->text(1) << ", ";                  // 오더 id, 날짜
        out << item->text(2) << ", " << item->text(3) << ", ";                  // 클라이언트 이름, 아이템 이름
        out << item->text(4) << ", " << item->text(5) << "\n";                  // 수량, 총액
    }

    file.close( );                                                              // 파일을 다 쓰고 파일 닫기

    delete ui;
}

void OrderManager::loadData()                                                   // 미리 저장된 클라이언트 데이터를 가져온다
{
    QFile file("orderlist.txt");                                                // 저장되어 있는 아이템 데이터 파일명
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))                      // 파일이 있다면 읽기전용으로 열기
        return;

    QTextStream in(&file);                                                  // 아이템 데이터가 담겨있는 파일을 텍스트스트림에 담기
    while (!in.atEnd()) {                                                   // 파일의 끝까지 가면서
        QString line = in.readLine();                                       // 한줄씩 읽는다
        QList<QString> row = line.split(", ");                              // 쉼표(,)로 구분하면서 한줄씩 파싱
        if(row.size()) {                                                    // 읽을 데이터가 남아있다면
            QTreeWidgetItem *item = new QTreeWidgetItem;                    // 데이터를 트리위젯아이템으로 저장
            item->setText(0,row[0]);                                        // 주문 번호
            item->setText(1,row[1]);                                        // 주문 날짜
            item->setText(2,row[2]);                                        // 클라이언트 이름
            item->setText(3,row[3]);                                        // 아이템 이름
            item->setText(4,row[4]);                                        // 주문 수량
            item->setText(5,row[5]);                                        // 총액
            ui->orderTreeWidget->addTopLevelItem(item);                     // 저장된 데이터를 정보 위젯에 올린다
            int id = row[0].toInt();                                        // 주문 번호
            int c_id = row[1].toInt();                                      // 클라이언트 이름
            int i_id = row[2].toInt();                                      // 아이템 이름
            int count = row[3].toInt();                                     // 주문 수량
            Order* o = new Order(id, c_id, i_id, count);                    // 오더 형태로 저장
            orderList.insert(id, o);                                        // 읽어온 데이터를 오더 리스트에 추가

        }
    }
    file.close( );                                                          // 파일을 다 읽으면 파일 닫기
}

int OrderManager::makeId( )                                                 // 주문 번호 생성
{
    if(orderList.size( ) == 0) {                                            // 등록된 주문이 없다면
        return 1;                                                           // id 1번 부여
    } else {                                                                // 등록된 주문이 있다면
        auto id = orderList.lastKey();                                      // 주문 리스트의 마지막 키값을 가지고
        return ++id;                                                        // 키값을 1 추가해서 새로운 id 생성
    }
}

void OrderManager::showClientData(Client* item)                             // 클라이언트 리스트 출력
{
    QString name, address, phoneNum, type;                                  // 이름, 주소, 전화번호, 타입
    int id;                                                                 // 클라이언트 ID
    id = item->id();                                                        // 클라이언트 ID
    name = item->getName();                                                 // 클라이언트 이름
    address = item->getAddress();                                           // 클라이언트 주소
    phoneNum = item->getPhoneNum();                                         // 클라이언트 전화번호
    type = item->getType();                                                 // 클라이언트 타입
    if(name.length()) {                                                     // 저장된 이름이 있다면
        Client* c = new Client(id, name, address, phoneNum, type);          // 클라이언트 형태로
        ui->clientTreeWidget->addTopLevelItem(c);                           // 트리위젯에 추가
    }
}

void OrderManager::on_clientLineEdit_returnPressed()                        // 클라이언트 조건 검색 입력창
{
    ui->clientTreeWidget->clear();                                          // 트리위젯 초기화

    int i = ui->clientComboBox->currentIndex();                             // 인덱스 번호를 가져와서

    if (i == 0) {                                                           // 인덱스가 0 이면
        int id = ui->clientLineEdit->text().toInt();                        // ID를
        emit clientDataSent(id);                                            // 넘겨준다
    }
    else if (i==1) {                                                        // 인덱스가 1이면
        QString name = ui->clientLineEdit->text();                          // 이름을
        emit clientDataSent(name);                                          // 넘겨준다
    }
    else if (i==2) {                                                        // 인덱스가 2면
        QString address = ui->clientLineEdit->text();                       // 주소를
        emit clientDataSent(address);                                       // 넘겨준다
    }
    else if (i==3) {                                                        // 인덱스가 3이면
        QString type = ui->clientLineEdit->text();                          // 타입을
        emit clientDataSent(type);                                          // 넘겨준다
    }
}

void OrderManager::showItemData(Item* item)                                 // 아이템 데이터 출력
{
    QString name, categori, color, stock, price;                            // 이름,카테고리,색상,재고량,가격
    int id;                                                                 // 아이템 ID
    id = item->id();                                                        // 아이템 ID
    name = item->getName();                                                 // 아이템 이름 가져오기
    categori = item->getCategori();                                         // 아이템 카테고리 가져오기
    color = item->getColor();                                               // 아이템 색상 가져오기
    stock = item->getStock();                                               // 아이템 재고량 가져오기
    price = item->getPrice();                                               // 아이템 가격 가져오기
    if(name.length()) {                                                     // 아이템이 등록되어 있다면
        Item* c = new Item(id, name, categori, color, stock, price);        // 아이템 형태로
        ui->itemTreeWidget->addTopLevelItem(c);                             // 트리위젯에 추가
    }
}


void OrderManager::on_itemLineEdit_returnPressed()                          // 아이템 조건 검색 입력창
{
    ui->itemTreeWidget->clear();                                            // 트리위젯 초기화

    int i = ui->itemComboBox->currentIndex();                               // 인덱스 번호를 가져와서

    if (i == 0) {                                                           // 인덱스가 0이면
        int id = ui->itemLineEdit->text().toInt();                          // ID를
        emit itemDataSent(id);                                              // 넘겨준다
    }
    else if (i==1) {                                                        // 인덱스가 1이면
        QString name = ui->itemLineEdit->text();                            // 이름을
        emit itemDataSent(name);                                            // 념겨준다
    }
    else if (i==2) {                                                        // 인덱스가 2이면
        QString categori = ui->itemLineEdit->text();                        // 카테고리를
        emit itemDataSent(categori);                                        // 넘겨준다
    }
    else if (i==3) {                                                        // 인덱스가 3이면
        QString color = ui->itemLineEdit->text();                           // 색상을
        emit itemDataSent(color);                                           // 념겨준다
    }
}


void OrderManager::on_clientTreeWidget_itemClicked(QTreeWidgetItem *item, int column)   // 아이템 리스트를 클릭하면
{
    Q_UNUSED(column);                                                                   // 현재 선택된 아이템을 넘기기 위한 column

    QTreeWidgetItem* c = ui->clientTreeWidget->currentItem();                           // 현재 아이템을 트리위젯 형태로
    if(c != nullptr)                                                                    // 현재 아이템이 있다면
    {
        ui->orderClientIdlineEdit->setText(item->text(0));                              // ID를 가져온다
    }
}


void OrderManager::on_itemTreeWidget_itemClicked(QTreeWidgetItem *item, int column)     // 아이템 리스트를 클릭하면
{
    Q_UNUSED(column);                                                                   // 현재 선택된 아이템을 넘기기 위한 column

    QTreeWidgetItem* c = ui->itemTreeWidget->currentItem();                             // 현재 아이템을 트리위젯 형태로
    if(c != nullptr)                                                                    // 현재 아이템이 있다면
    {
        ui->orderItemIdlineEdit->setText(item->text(0));                                // ID를 가져온다
        ui->orderPricelineEdit->setText(item->text(5));                                 // 가격을 가져온다
    }
}


void OrderManager::on_orderQuantitylineEdit_returnPressed()                             // 수량 입력창을 누르고 엔터를 누르면
{
    int q = ui->orderQuantitylineEdit->text().toInt();                                  // 수량 저장

    int amount = ui->orderPricelineEdit->text().toInt() * q;                            // 가격과 수량을 곱해서 총액 결정

    ui->orderAmountlineEdit->setText(QString::number(amount));                          // 총액을 설정
}


void OrderManager::on_orderPushButton_clicked()                                         // order 버튼
{
    QTreeWidgetItem* row = new QTreeWidgetItem(ui->orderTreeWidget);                    // 트리위젯 형태로 저장

    int orderNum = makeId();                                                            // 주문 번호 생성

    date = QTime::currentTime().toString();                                             // 주문 날짜 저장

    int c_id = ui->orderClientIdlineEdit->text().toInt();                               // 클라이언트 ID
    int i_id = ui->orderItemIdlineEdit->text().toInt();                                 // 아이템 ID
    emit clientNameDataSent(c_id,row);                                                  // 클라이언트 ID 전송
    emit itemNameDataSent(i_id,row);                                                    // 아이템 ID 전송

    int q = ui->orderQuantitylineEdit->text().toInt();                                  // 수량 설정

    int amount = ui->orderPricelineEdit->text().toInt() * q;                            // 총액 설정

    Order* order = new Order(orderNum,c_id,i_id,q);                                     // 오더 형태로
    orderList.insert(orderNum,order);                                                   // 오더 리스트에 추가

    row->setText(0,QString::number(orderNum));                                          // 주문 번호
    row->setText(1,date);                                                               // 주문 날짜
    row->setText(4,QString::number(q));                                                 // 주문 수량
    row->setText(5,QString::number(amount));                                            // 주문 총액

    QTreeWidget* item = new QTreeWidget(ui->orderTreeWidget);                           // 오더리스트에 출력
    saveOrderList.insert(orderNum,item);                                                // 오더리스트에 추가

    ui->orderClientIdlineEdit->clear();                                                 // 주문 입력창 초기화
    ui->orderItemIdlineEdit->clear();                                                   // 주문 입력창 초기화
    ui->orderQuantitylineEdit->clear();                                                 // 주문 입력창 초기화
    ui->orderPricelineEdit->clear();                                                    // 주문 입력창 초기화
    ui->orderAmountlineEdit->clear();                                                   // 주문 입력창 초기화
}

void OrderManager::showClientNameData(Client* client,QTreeWidgetItem* row)              // 클라이언트 형태로 데이터 출력
{
    QString name, address, phoneNum, type;                                              // 이름, 주소, 전화번호, 타입
    int id;                                                                             // 클라이언트 ID
    id = client->id();                                                                  // 클라이언트 ID
    name = client->getName();                                                           // 클라이언트 이름
    address = client->getAddress();                                                     // 클라이언트 주소
    phoneNum = client->getPhoneNum();                                                   // 클라이언트 전화번호
    type = client->getType();                                                           // 클라이언트 타입
    if(name.length()) {                                                                 // 등록된 클라이언트가 있다면
        row->setText(2,name);                                                           // 오더 리스트에 이름 출력
    }
}

void OrderManager::showItemNameData(Item* item,QTreeWidgetItem* row)                    // 아이템 형태로 데이터 출력
{
    QString name, categori, color, stock, price;                                        // 이름, 카테고리,색상,재고량,가격
    int id;                                                                             // 아이템 ID
    id = item->id();                                                                    // 아이템 ID
    name = item->getName();                                                             // 아이템 이름
    categori = item->getCategori();                                                     // 아이템 카테고리
    color = item->getColor();                                                           // 아이템 색상
    stock = item->getStock();                                                           // 아이템 재고량
    price = item->getPrice();                                                           // 아이템 가격
    if(name.length()) {                                                                 // 등록된 아이템이 있다면
        row->setText(3,name);                                                           // 오더리스트에 출력
    }
}

void OrderManager::on_SearchPushButton_clicked()                                        // search 버튼
{
    ui->searchTreeWidget->clear();                                                      // 트리위젯 초기화

    int i = ui->SearchComboBox->currentIndex();                                         // 검색할 조건을 설정
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains                           // id로 검색하면 정확한 값, 나머지는 대소문자 구분
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->orderTreeWidget->findItems(ui->searchLineEdit->text(),flag,i); // 조회할 아이템을 가져온다

        foreach(auto i, items)                                                          // 아이템을 돌아가면서
        {
            Order* o = static_cast<Order*>(i);                                          // 오더 형태로 만들고
            int orderNum = o->orderNum();                                               // 주문 id 가져오기
            ui->searchTreeWidget->addTopLevelItem(orderList[orderNum]);                 // 저장한 아이템을 트리위젯에 추가
        }
    }
}


void OrderManager::on_clearPushButton_clicked()                                         // clear 버튼
{
    ui->orderClientIdlineEdit->clear();                                                 // 입력창 초기화
    ui->orderItemIdlineEdit->clear();                                                   // 입력창 초기화
    ui->orderQuantitylineEdit->clear();                                                 // 입력창 초기화
    ui->orderPricelineEdit->clear();                                                    // 입력창 초기화
    ui->orderAmountlineEdit->clear();                                                   // 입력창 초기화
}


void OrderManager::on_orderTreeWidget_itemClicked(QTreeWidgetItem *item, int column)    // 아이템 리스트를 클릭하면
{
    QTreeWidgetItem* c = ui->orderTreeWidget->currentItem();                            // 현재 아이템을 트리위젯 형태로
    if (c != nullptr)                                                                   // 현재 아이템이 있다면
    {
        Q_UNUSED(column);                                                               // 현재 선택된 아이템을 넘기기 위한 column
        ui->orderClientIdlineEdit->clear();                                             // 입력창 초기화
        ui->orderItemIdlineEdit->clear();                                               // 입력창 초기화
        ui->orderPricelineEdit->clear();                                                // 입력창 초기화
        ui->orderQuantitylineEdit->clear();                                             // 입력창 초기화
        ui->orderAmountlineEdit->clear();                                               // 입력창 초기화

        int c_id = orderList.value(item->text(0).toInt())->getClientId();               // 클라이언트 ID
        int i_id = orderList.value(item->text(0).toInt())->getItemId();                 // 아이템 ID

        int q = (item->text(5).toInt() / item->text(4).toInt());                        // 주문 수량

        ui->orderClientIdlineEdit->setText(QString::number(c_id));                      // 클라이언트 ID 설정
        ui->orderItemIdlineEdit->setText(QString::number(i_id));                        // 아이템 ID 설정
        ui->orderPricelineEdit->setText(QString::number(q));                            // 아이템 가격 설정
        ui->orderQuantitylineEdit->setText(item->text(4));                              // 주문 수량 설정
        ui->orderAmountlineEdit->setText(item->text(5));                                // 주문 총액 설정
    }
}

void OrderManager::on_RemovePushButton_clicked()                                        // remove 버튼
{
    QTreeWidgetItem* item = ui->orderTreeWidget->currentItem();                         // 선택된 아이템
    if(item != nullptr) {                                                               // 이 있다면
        orderList.remove(item->text(0).toInt());                                        // 오더 리스트에서 제거
        ui->orderTreeWidget->takeTopLevelItem(ui->orderTreeWidget->indexOfTopLevelItem(item));  // 오더 리스트 출력에서 제거
        ui->orderTreeWidget->update();                                                  // 오더 리스트 업데이트
    }
}

