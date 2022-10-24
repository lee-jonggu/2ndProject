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

    QList<int> sizes;
    sizes << 300 << 660;
    ui->splitter->setSizes(sizes);

    connect(ui->ItemTreeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(showItem(QTreeWidgetItem*,int)));

}

ItemManager::~ItemManager()
{
    delete ui;

    QFile file("itemlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : itemList) {
        Item* i = v;
        out << i->id() << ", " << i->getName() << ", ";
        out << i->getCategori() << ", ";
        out << i->getColor() << ", ";
        out << i->getStock() << ", ";
        out << i->getPrice() << "\n";
    }
    file.close( );
}

void ItemManager::loadData()
{
    QFile file("itemlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            Item* i = new Item(id, row[1], row[2], row[3], row[4], row[5]);
            ui->ItemTreeWidget->addTopLevelItem(i);
            itemList.insert(id, i);

            emit itemAdded(row[1]);
        }
    }
    file.close( );
}

int ItemManager::makeId( )
{
    if(itemList.size( ) == 0) {
        return 880000;
    } else {
        auto id = itemList.lastKey();
        return ++id;
    }
}

void ItemManager::on_AddPushButton_clicked()
{
    QString name, categori, color, stock, price;
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
