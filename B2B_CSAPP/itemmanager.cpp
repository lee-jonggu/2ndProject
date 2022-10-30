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
    int id = makeId( );
    name = ui->InputNameLineEdit->text();
    categori = ui->InputCategoriLineEdit->text();
    color = ui->InputColorLineEdit->text();
    stock = ui->InputStockLineEdit->text();
    price = ui->priceLineEdit->text();
    if(name.length()) {
        Item* i = new Item(id, name, categori, color, stock, price);
        itemList.insert(id, i);
        ui->ItemTreeWidget->addTopLevelItem(i);
        emit itemAdded(name);
    }

    addLogList.insert(id,QTime::currentTime().toString());

    ui->TimeListWidget->clear();

    ui->InputNameLineEdit->clear();
    ui->InputCategoriLineEdit->clear();
    ui->InputColorLineEdit->clear();
    ui->InputStockLineEdit->clear();
    ui->priceLineEdit->clear();

    ui->TimeListWidget->addItem("Added Time : "+QTime::currentTime().toString());
}


void ItemManager::on_ModifyPushButton_clicked()
{
    QTreeWidgetItem* item = ui->ItemTreeWidget->currentItem();
    if(item != nullptr) {
        int key = item->text(0).toInt();
        Item* c = itemList[key];
        QString name, categori, color, stock, price;
        name = ui->InputNameLineEdit->text();
        categori = ui->InputCategoriLineEdit->text();
        color = ui->InputColorLineEdit->text();
        stock = ui->InputStockLineEdit->text();
        price = ui->priceLineEdit->text();
        c->setName(name);
        c->setCategori(categori);
        c->setColor(color);
        c->setStock(stock);
        c->setPrice(price);
        itemList[key] = c;

        logTimeList.insert(key,QTime::currentTime().toString());
    }
}


void ItemManager::on_RemovePushButton_clicked()
{
    QTreeWidgetItem* item = ui->ItemTreeWidget->currentItem();
    if(item != nullptr) {
        itemList.remove(item->text(0).toInt());
        ui->ItemTreeWidget->takeTopLevelItem(ui->ItemTreeWidget->indexOfTopLevelItem(item));
        ui->ItemTreeWidget->update();
    }
}


void ItemManager::on_SearchPushButton_clicked()
{
    ui->ItemSearchTreeWidget->clear();

    int i = ui->SearchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->ItemTreeWidget->findItems(ui->InputLineEdit->text(), flag, i);

        foreach(auto i, items) {
            Item* t = static_cast<Item*>(i);
            int id = t->id();
            QString name = t->getName();
            QString categori = t->getCategori();
            QString color = t->getColor();
            QString stock = t->getStock();
            QString price = t->getPrice();
            Item* item = new Item(id, name, categori, color, stock, price);
            ui->ItemSearchTreeWidget->addTopLevelItem(item);
        }
    }
}



void ItemManager::showItem(QTreeWidgetItem* item,int i)
{
    QTreeWidgetItem* c = ui->ItemTreeWidget->currentItem();
    if(c != nullptr)
    {
        ui->TimeListWidget->clear();

        Q_UNUSED(i);
        ui->InputIdLineEdit->setText(item->text(0));
        ui->InputNameLineEdit->setText(item->text(1));
        ui->InputCategoriLineEdit->setText(item->text(2));
        ui->InputColorLineEdit->setText(item->text(3));
        ui->InputStockLineEdit->setText(item->text(4));
        ui->priceLineEdit->setText(item->text(5));

        int key = item->text(0).toInt();
        for (auto& v : logTimeList.values(key))
        {
            ui->TimeListWidget->addItem("Modified Time : "+v);
        }
        ui->TimeListWidget->addItem("Added Time : "+addLogList[key]);
    }
}

void ItemManager::itemIdListData(int id)
{
    auto items = ui->ItemTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {
        Item* t = static_cast<Item*>(i);
        int id = t->id();
        QString name = t->getName();
        QString categori = t->getCategori();
        QString color = t->getColor();
        QString stock = t->getStock();
        QString price = t->getPrice();
        Item* item = new Item(id, name, categori, color, stock, price);
        emit itemDataSent(item);
    }
}


void ItemManager::itemNameListData(QString istr)
{
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,1);

    foreach(auto i, items) {
        Item* t = static_cast<Item*>(i);
        int id = t->id();
        QString name = t->getName();
        QString categori = t->getCategori();
        QString color = t->getColor();
        QString stock = t->getStock();
        QString price = t->getPrice();
        Item* item = new Item(id, name, categori, color, stock, price);
        emit itemDataSent(item);
    }
}

void ItemManager::itemCategoriListData(QString istr)
{
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,2);

    foreach(auto i, items) {
        Item* t = static_cast<Item*>(i);
        int id = t->id();
        QString name = t->getName();
        QString categori = t->getCategori();
        QString color = t->getColor();
        QString stock = t->getStock();
        QString price = t->getPrice();
        Item* item = new Item(id, name, categori, color, stock, price);
        emit itemDataSent(item);
    }
}

void ItemManager::itemColorListData(QString istr)
{
    auto items = ui->ItemTreeWidget->findItems(istr,Qt::MatchContains,3);

    foreach(auto i, items) {
        Item* t = static_cast<Item*>(i);
        int id = t->id();
        QString name = t->getName();
        QString categori = t->getCategori();
        QString color = t->getColor();
        QString stock = t->getStock();
        QString price = t->getPrice();
        Item* item = new Item(id, name, categori, color, stock, price);
        emit itemDataSent(item);
    }
}

void ItemManager::itemIdNameListData(int id,QTreeWidgetItem* row)
{
    Q_UNUSED(row);
    auto items = ui->ItemTreeWidget->findItems(QString::number(id),Qt::MatchContains | Qt::MatchCaseSensitive,0);

    foreach(auto i, items) {
        Item* t = static_cast<Item*>(i);
        QString name = t->getName();
        QString categori = t->getCategori();
        QString color = t->getColor();
        QString stock = t->getStock();
        QString price = t->getPrice();
        Item* item = new Item(id, name, categori, color, stock, price);
        emit itemNameDataSent(item,row);
    }
}
