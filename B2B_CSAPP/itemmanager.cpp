#include "itemmanager.h"
#include "ui_itemmanager.h"
#include "item.h"

#include <QFile>
#include <QTime>

ItemManager::ItemManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemManager)
{
    ui->setupUi(this);

    QList<int> sizes;                                                       // ui의 스플리터 사이즈 조절을 위한 사이즈
    sizes << 300 << 660;                                                    // 사이즈 설정
    ui->splitter->setSizes(sizes);                                          // ui 스플리터 사이즈 설정

    /* 등록된 아이템 클릭하면 아이템 정보를 입력하는 창에 아이템 표시 */
    connect(ui->ItemTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showItem(QTreeWidgetItem*,int)));

}

ItemManager::~ItemManager()                                                 // 소멸자에서 데이터 저장
{
    delete ui;

    QFile file("itemlist.txt");                                             // 텍스트파일 이름
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                 // 읽을 파일이 없다면 리턴
        return;

    QTextStream out(&file);                                                 // 텍스트스트림에 담을 파일 생성
    for (const auto& v : itemList) {                                        // 저장되어있는 아이템 리스트 돌면서
        Item* i = v;                                                        // 아이템 형태로
        out << i->id() << ", " << i->getName() << ", ";                     // 아이템 id, 이름
        out << i->getCategori() << ", ";                                    // 아이템 카테고리
        out << i->getColor() << ", ";                                       // 아이템 색상
        out << i->getStock() << ", ";                                       // 아이템 재고량
        out << i->getPrice() << "\n";                                       // 클라이언트 타입 저장
    }
    file.close( );                                                          // 파일을 다 쓰고 파일 닫기
}

void ItemManager::loadData()                                                // 미리 저장된 클라이언트 데이터를 가져온다
{
    QFile file("itemlist.txt");                                             // 저장되어 있는 아이템 데이터 파일명
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))                  // 파일이 있다면 읽기전용으로 열기
        return;

    QTextStream in(&file);                                                  // 아이템 데이터가 담겨있는 파일을 텍스트스트림에 담기
    while (!in.atEnd()) {                                                   // 파일의 끝까지 가면서
        QString line = in.readLine();                                       // 한줄씩 읽는다
        QList<QString> row = line.split(", ");                              // 쉼표(,)로 구분하면서 한줄씩 파싱
        if(row.size()) {                                                    // 읽을 데이터가 남아있다면
            int id = row[0].toInt();                                        // 첫번째 파싱을 클라이언트 id
            Item* i = new Item(id, row[1], row[2], row[3], row[4], row[5]); // 나머지 데이터를 아이템 형태로 저장
            ui->ItemTreeWidget->addTopLevelItem(i);                         // 저장된 데이터를 정보 위젯에 올린다
            itemList.insert(id, i);                                         // 읽어온 데이터를 아이템 리스트에 추가

            emit itemAdded(row[1]);                                         // 아이템이 추가될 때 시그널 발생
        }
    }
    file.close( );                                                          // 파일을 다 읽으면 파일 닫기
}

int ItemManager::makeId( )                                                  // 아이템 id 생성
{
    if(itemList.size( ) == 0) {                                             // 등록된 클라이언트가 없다면
        return 880000;                                                      // id 1000번 부여
    } else {                                                                // 등록된 클라이언트가 있다면
        auto id = itemList.lastKey();                                       // 클라이언트 리스트의 마지막 키값을 가지고
        return ++id;                                                        // 키값을 1 추가해서 새로운 id 생성
    }
}

void ItemManager::on_AddPushButton_clicked()                                // add 버튼
{
    QString name, categori, color, stock, price;                            // 이름, 카테고리, 색상, 재고량, 가격
    int id = makeId( );                                                     // 클라이언트 id는 매번 새로 생성
    name = ui->InputNameLineEdit->text();                                   // 라인에딧에 있는 이름 사용
    categori = ui->InputCategoriLineEdit->text();                           // 라인에딧에 있는 카테고리 사용
    color = ui->InputColorLineEdit->text();                                 // 라인에딧에 있는 색상 사용
    stock = ui->InputStockLineEdit->text();                                 // 라인에딧에 있는 재고량 사용
    price = ui->priceLineEdit->text();                                      // 라인에딧에 있는 가격 사용
    if(name.length()) {                                                     // 아이템 이름이 비어있지 않으면
        Item* i = new Item(id, name, categori, color, stock, price);        // 아이템 형태로 객체 생성
        itemList.insert(id, i);                                             // 아이템 리스트에 저장
        ui->ItemTreeWidget->addTopLevelItem(i);                             // 아이템 리스트에 출력
        emit itemAdded(name);                                               // 아이템이 추가될 때 시그널 발생
    }

    addLogList.insert(id,QTime::currentTime().toString());                  // 로그를 관리하기 위해 로그 리스트에 저장

    ui->TimeListWidget->clear();                                            // 리스트를 저장하고 입력창 초기화
    ui->InputNameLineEdit->clear();                                         // 리스트를 저장하고 입력창 초기화
    ui->InputCategoriLineEdit->clear();                                     // 리스트를 저장하고 입력창 초기화
    ui->InputColorLineEdit->clear();                                        // 리스트를 저장하고 입력창 초기화
    ui->InputStockLineEdit->clear();                                        // 리스트를 저장하고 입력창 초기화
    ui->priceLineEdit->clear();                                             // 리스트를 저장하고 입력창 초기화

    ui->TimeListWidget->addItem("Added Time : "+QTime::currentTime().toString()); // 로그창에 출력
}


void ItemManager::on_ModifyPushButton_clicked()                             // modify 버튼 클릭
{
    QTreeWidgetItem* item = ui->ItemTreeWidget->currentItem();              // 저장되어 있는 리스트에서 수정할 아이템 클릭
    if(item != nullptr) {                                                   // 아이템이 있다면
        int key = item->text(0).toInt();                                    // 아이템 id로 key 설정
        Item* c = itemList[key];                                            // 아이템 리스트를 아이템ㅎ id로 관리
        QString name, categori, color, stock, price;                        // 이름, 카테고리, 색상, 재고량, 가격
        name = ui->InputNameLineEdit->text();                               // 수정할 이름 쓰기
        categori = ui->InputCategoriLineEdit->text();                       // 수정할 카테고리 쓰기
        color = ui->InputColorLineEdit->text();                             // 수정할 색상 쓰기
        stock = ui->InputStockLineEdit->text();                             // 수정할 재고량 쓰기
        price = ui->priceLineEdit->text();                                  // 수정할 가격 쓰기
        c->setName(name);                                                   // 새로운 이름으로 수정
        c->setCategori(categori);                                           // 새로운 카테고리로 수정
        c->setColor(color);                                                 // 새로운 색상으로 수정
        c->setStock(stock);                                                 // 새로운 재고량으로 수정
        c->setPrice(price);                                                 // 새로운 가격으로 수정
        itemList[key] = c;                                                  // 바뀐 정보를 가지고 클라이언트 리스트 덮어쓰기

        logTimeList.insert(key,QTime::currentTime().toString());            // 바뀐 정보를 로그에 찍기
    }
}


void ItemManager::on_RemovePushButton_clicked()                             // remove 버튼 클릭
{
    QTreeWidgetItem* item = ui->ItemTreeWidget->currentItem();              // 저장되어 있는 리스트에서 삭제할 아이템 클릭
    if(item != nullptr) {                                                   // 아이템이 있다면
        itemList.remove(item->text(0).toInt());                             // 아이템 리스트에서 id를 가지고 삭제
        ui->ItemTreeWidget->takeTopLevelItem(ui->ItemTreeWidget->indexOfTopLevelItem(item));    // 인덱스를 가지고 트리위젯에서 삭제
        ui->ItemTreeWidget->update();                                                           // 삭제 후 리스트 다시 보여주기
    }
}


void ItemManager::on_SearchPushButton_clicked()                             // search 버튼 클릭
{
    ui->ItemSearchTreeWidget->clear();                                      // 조회한 트리위젯 초기화 후

    int i = ui->SearchComboBox->currentIndex();                             // 검색할 조건을 설정
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains               // id로 검색하면 정확한 값, 나머지는 대소문자 구분
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->ItemTreeWidget->findItems(ui->InputLineEdit->text(), flag, i);   // 조회할 아이템을 가져온다

        foreach(auto i, items) {                                            // 아이템을 돌아가면서
            Item* t = static_cast<Item*>(i);                                // 아이템 형태로 만들고
            int id = t->id();                                               // 아이템 id 가져오기
            QString name = t->getName();                                    // 아이템 이름 가져오기
            QString categori = t->getCategori();                            // 아이템 카테고리 가져오기
            QString color = t->getColor();                                  // 아이템 색상 가져오기
            QString stock = t->getStock();                                  // 아이템 재고량 가져오기
            QString price = t->getPrice();                                  // 아이템 가격 가져오기
            Item* item = new Item(id, name, categori, color, stock, price); // 가져온 정보를을 아이템 형태로 저장
            ui->ItemSearchTreeWidget->addTopLevelItem(item);                // 저장한 아이템을 트리위젯에 추가
        }
    }
}



void ItemManager::showItem(QTreeWidgetItem* item,int i)                     // 클라이언트 리스트에 있는 클라이언트를 클릭하면 입력창에 보인다
{
    QTreeWidgetItem* c = ui->ItemTreeWidget->currentItem();                 // 현재 선택한 클라이언트
    if(c != nullptr)                                                        // 가 있다면
    {
        ui->TimeListWidget->clear();                                        // 기존 타임 리스트 삭제

        Q_UNUSED(i);                                                        // 인덱스는 쓰지 않고
        ui->InputIdLineEdit->setText(item->text(0));                        // 선택된 아이템 아이디를 입력창에 출력
        ui->InputNameLineEdit->setText(item->text(1));                      // 선택된 아이템 이름을 입력창에 출력
        ui->InputCategoriLineEdit->setText(item->text(2));                  // 선택된 아이템 카테고리를 입력창에 출력
        ui->InputColorLineEdit->setText(item->text(3));                     // 선택된 아이템 색상을 입력창에 출력
        ui->InputStockLineEdit->setText(item->text(4));                     // 선택된 아이템 재고량을 입력창에 출력
        ui->priceLineEdit->setText(item->text(5));                          // 선택된 아이템 가격을 입력창에 출력

        int key = item->text(0).toInt();                                    // 보여준 아이템 id 저장
        for (auto& v : logTimeList.values(key))                             // id를 가지고 아이템 형태로 저장
        {
            ui->TimeListWidget->addItem("Modified Time : "+v);              // 로그창에 수정된 시간, 정보 출력
        }
        ui->TimeListWidget->addItem("Added Time : "+addLogList[key]);       // 아이템이 처음 추가될 때 로그를 출력
    }
}

void ItemManager::itemIdListData(int id)                                    // 아이템 id를 가지고 클라이언트 정보 추출
{
    /* 클라이언트 id를 이용하여 */
    auto items = ui->ItemTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {                                                // 저장된 아이템 리스트를 돌면서
        Item* t = static_cast<Item*>(i);                                    // 아이템 형태로 저장
        int id = t->id();                                                   // 아이템 id 가져오기
        QString name = t->getName();                                        // 아이템 이름 가져오기
        QString categori = t->getCategori();                                // 아이템 카테고리 가져오기
        QString color = t->getColor();                                      // 아이템 색상 가져오기
        QString stock = t->getStock();                                      // 아이템 재고량 가져오기
        QString price = t->getPrice();                                      // 아이템 가격 가져오기
        Item* item = new Item(id, name, categori, color, stock, price);     // 아이템 형태로 객체 저장
        emit itemDataSent(item);                                            // 저장된 객체를 시그널로 전송
    }
}


void ItemManager::itemNameListData(QString istr)                            // 아이템 이름을 가지고 아이템 정보 추출
{
    /* 클라이언트 이름을 이용하여 */
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,1);

    foreach(auto i, items) {                                                // 저장된 아이템 리스트를 돌면서
        Item* t = static_cast<Item*>(i);                                    // 아이템 형태로 저장
        int id = t->id();                                                   // 아이템 id 가져오기
        QString name = t->getName();                                        // 아이템 이름 가져오기
        QString categori = t->getCategori();                                // 아이템 카테고리 가져오기
        QString color = t->getColor();                                      // 아이템 색상 가져오기
        QString stock = t->getStock();                                      // 아이템 재고량 가져오기
        QString price = t->getPrice();                                      // 아이템 가격 가져오기
        Item* item = new Item(id, name, categori, color, stock, price);     // 아이템 형태로 객체 저장
        emit itemDataSent(item);                                            // 저장된 객체를 시그널로 전송
    }
}

void ItemManager::itemCategoriListData(QString istr)                        // 아이템 카테고리를 가지고 아이템 정보 추출
{
    /* 클라이언트 이름을 이용하여 */
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,2);

    foreach(auto i, items) {                                                // 아이템 형태로 저장
        Item* t = static_cast<Item*>(i);                                    // 아이템 형태로 저장
        int id = t->id();                                                   // 아이템 id 가져오기
        QString name = t->getName();                                        // 아이템 이름 가져오기
        QString categori = t->getCategori();                                // 아이템 카테고리 가져오기
        QString color = t->getColor();                                      // 아이템 색상 가져오기
        QString stock = t->getStock();                                      // 아이템 재고량 가져오기
        QString price = t->getPrice();                                      // 아이템 가격 가져오기
        Item* item = new Item(id, name, categori, color, stock, price);     // 아이템 형태로 객체 저장
        emit itemDataSent(item);                                            // 저장된 객체를 시그널로 전송
    }
}

void ItemManager::itemColorListData(QString istr)                            // 아이템 색상을 가지고 아이템 정보 추출
{
    /* 클라이언트 이름을 이용하여 */
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,3);

    foreach(auto i, items) {                                                // 저장된 아이템 리스트를 돌면서
        Item* t = static_cast<Item*>(i);                                    // 아이템 형태로 저장
        int id = t->id();                                                   // 아이템 id 가져오기
        QString name = t->getName();                                        // 아이템 이름 가져오기
        QString categori = t->getCategori();                                // 아이템 카테고리 가져오기
        QString color = t->getColor();                                      // 아이템 색상 가져오기
        QString stock = t->getStock();                                      // 아이템 재고량 가져오기
        QString price = t->getPrice();                                      // 아이템 가격 가져오기
        Item* item = new Item(id, name, categori, color, stock, price);     // 아이템 형태로 객체 저장
        emit itemDataSent(item);                                            // 저장된 객체를 시그널로 전송
    }
}

void ItemManager::itemIdNameListData(int id,QTreeWidgetItem* row)           // 아이템 id를 가지고 트리위젯아이템 형태로 추출
{
    Q_UNUSED(row);                                                          // 오더폼에서 사용하는 행
    /* 클라이언트 id를 이용하여 */
    auto items = ui->ItemTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {                                                // 저장된 아이템 리스트를 돌면서
        Item* t = static_cast<Item*>(i);                                    // 아이템 형태로 저장
        QString name = t->getName();                                        // 아이템 이름 가져오기
        QString categori = t->getCategori();                                // 아이템 카테고리 가져오기
        QString color = t->getColor();                                      // 아이템 색상 가져오기
        QString stock = t->getStock();                                      // 아이템 재고량 가져오기
        QString price = t->getPrice();                                      // 아이템 가격 가져오기
        Item* item = new Item(id, name, categori, color, stock, price);     // 아이템 형태로 객체 저장
        emit itemNameDataSent(item,row);                                    // 저장된 객체를 시그널로 전송
    }
}
