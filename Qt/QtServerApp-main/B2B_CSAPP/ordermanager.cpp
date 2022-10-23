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

    QList<int> sizes;
    sizes << 450 << 630;
    ui->splitter->setSizes(sizes);
}

OrderManager::~OrderManager()
{
    delete ui;

    QFile file("orderlist.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    for (const auto& v : saveOrderList) {
        QTreeWidget* i = v;
//        out << i->statusTip(0) << ", " ;
//        out << i->statusTip(1) << ", ";
//        out << i->statusTip(2) << ", ";
//        out << i->statusTip(3) << ", ";
//        out << i->statusTip(4) << ", ";
//        out << i->statusTip(5) << "\n";

    }

    file.close( );
}

void OrderManager::loadData()
{
    QFile file("orderlist.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QList<QString> row = line.split(", ");
        if(row.size()) {
            int id = row[0].toInt();
            int c_id = row[1].toInt();
            int i_id = row[2].toInt();
            int count = row[3].toInt();
            Order* o = new Order(id, c_id, i_id, count);
            ui->orderTreeWidget->addTopLevelItem(o);
            orderList.insert(id, o);

        }
    }
    file.close( );
}

int OrderManager::makeId( )
{
    if(orderList.size( ) == 0) {
        return 1;
    } else {
        auto id = orderList.lastKey();
        return ++id;
    }
}

void OrderManager::showClientData(Client* item)
{
    QString name, address, phoneNum, type;
    int id;
    id = item->id();
    name = item->getName();
    address = item->getAddress();
    phoneNum = item->getPhoneNum();
    type = item->getType();
    if(name.length()) {
        Client* c = new Client(id, name, address, phoneNum, type);
        ui->clientTreeWidget->addTopLevelItem(c);
    }
}

void OrderManager::on_clientLineEdit_returnPressed()
{
    ui->clientTreeWidget->clear();

    int i = ui->clientComboBox->currentIndex();

    if (i == 0) {
        int id = ui->clientLineEdit->text().toInt();
        emit clientDataSent(id);
    }
    else if (i==1) {
        QString name = ui->clientLineEdit->text();
        emit clientDataSent(name);
    }
    else if (i==2) {
        QString address = ui->clientLineEdit->text();
        emit clientDataSent(address);
    }
    else if (i==3) {
        QString type = ui->clientLineEdit->text();
        emit clientDataSent(type);
    }
}

void OrderManager::showItemData(Item* item)
{
    QString name, categori, color, stock, price;
    int id;
    id = item->id();
    name = item->getName();
    categori = item->getCategori();
    color = item->getColor();
    stock = item->getStock();
    price = item->getPrice();
    if(name.length()) {
        Item* c = new Item(id, name, categori, color, stock, price);
        ui->itemTreeWidget->addTopLevelItem(c);
    }
}


void OrderManager::on_itemLineEdit_returnPressed()
{
    ui->itemTreeWidget->clear();

    int i = ui->itemComboBox->currentIndex();

    if (i == 0) {
        int id = ui->itemLineEdit->text().toInt();
        emit itemDataSent(id);
    }
    else if (i==1) {
        QString name = ui->itemLineEdit->text();
        emit itemDataSent(name);
    }
    else if (i==2) {
        QString categori = ui->itemLineEdit->text();
        emit itemDataSent(categori);
    }
    else if (i==3) {
        QString color = ui->itemLineEdit->text();
        emit itemDataSent(color);
    }
}


void OrderManager::on_clientTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QTreeWidgetItem* c = ui->clientTreeWidget->currentItem();
    if(c != nullptr)
    {
        ui->orderClientIdlineEdit->setText(item->text(0));
    }
}


void OrderManager::on_itemTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    QTreeWidgetItem* c = ui->itemTreeWidget->currentItem();
    if(c != nullptr)
    {
        ui->orderItemIdlineEdit->setText(item->text(0));
        ui->orderPricelineEdit->setText(item->text(5));
    }
}


void OrderManager::on_orderQuantitylineEdit_returnPressed()
{
    int q = ui->orderQuantitylineEdit->text().toInt();

    int amount = ui->orderPricelineEdit->text().toInt() * q;

    ui->orderAmountlineEdit->setText(QString::number(amount));
}


void OrderManager::on_orderPushButton_clicked()
{
    QTreeWidgetItem* row = new QTreeWidgetItem(ui->orderTreeWidget);

    // Order Number
    int orderNum = makeId();

    // Date
    date = QTime::currentTime().toString();

    // Client Name, Item Name
    int c_id = ui->orderClientIdlineEdit->text().toInt();
    int i_id = ui->orderItemIdlineEdit->text().toInt();
    emit clientNameDataSent(c_id,row);
    emit itemNameDataSent(i_id,row);

    // Qunatity
    int q = ui->orderQuantitylineEdit->text().toInt();

    // Amount
    int amount = ui->orderPricelineEdit->text().toInt() * q;

    Order* order = new Order(orderNum,c_id,i_id,q);
    orderList.insert(orderNum,order);

    row->setText(0,QString::number(orderNum));
    row->setText(1,date);
    row->setText(4,QString::number(q));
    row->setText(5,QString::number(amount));

//    QTreeWidgetItem* item = new QTreeWidgetItem(ui->orderTreeWidget);
    QTreeWidget* item = new QTreeWidget(ui->orderTreeWidget);
    saveOrderList.insert(orderNum,item);
    qDebug() << saveOrderList[orderNum]->takeTopLevelItem(0)->text(0);

    ui->orderClientIdlineEdit->clear();
    ui->orderItemIdlineEdit->clear();
    ui->orderQuantitylineEdit->clear();
    ui->orderPricelineEdit->clear();
    ui->orderAmountlineEdit->clear();
}

void OrderManager::showClientNameData(Client* client,QTreeWidgetItem* row)
{
    QString name, address, phoneNum, type;
    int id;
    id = client->id();
    name = client->getName();
    address = client->getAddress();
    phoneNum = client->getPhoneNum();
    type = client->getType();
    if(name.length()) {
        row->setText(2,name);
    }
}

void OrderManager::showItemNameData(Item* item,QTreeWidgetItem* row)
{
    QString name, categori, color, stock, price;
    int id;
    id = item->id();
    name = item->getName();
    categori = item->getCategori();
    color = item->getColor();
    stock = item->getStock();
    price = item->getPrice();
    if(name.length()) {
        row->setText(3,name);
    }
}

void OrderManager::on_SearchPushButton_clicked()
{
    ui->searchTreeWidget->clear();

    int i = ui->SearchComboBox->currentIndex();
    auto flag = (i)? Qt::MatchCaseSensitive|Qt::MatchContains
                   : Qt::MatchCaseSensitive;
    {
        auto items = ui->orderTreeWidget->findItems(ui->searchLineEdit->text(),flag,i);

        foreach(auto i, items)
        {
            Order* o = static_cast<Order*>(i);
            int orderNum = o->orderNum();
            ui->searchTreeWidget->addTopLevelItem(orderList[orderNum]);
        }
    }

}


void OrderManager::on_clearPushButton_clicked()
{
    ui->orderClientIdlineEdit->clear();
    ui->orderItemIdlineEdit->clear();
    ui->orderQuantitylineEdit->clear();
    ui->orderPricelineEdit->clear();
    ui->orderAmountlineEdit->clear();
}


void OrderManager::on_orderTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem* c = ui->orderTreeWidget->currentItem();
    if (c != nullptr)
    {
        Q_UNUSED(column);
        ui->orderClientIdlineEdit->clear();
        ui->orderItemIdlineEdit->clear();
        ui->orderPricelineEdit->clear();
        ui->orderQuantitylineEdit->clear();
        ui->orderAmountlineEdit->clear();

        int c_id = orderList.value(item->text(0).toInt())->getClientId();
        int i_id = orderList.value(item->text(0).toInt())->getItemId();

        int q = (item->text(5).toInt() / item->text(4).toInt());

        ui->orderClientIdlineEdit->setText(QString::number(c_id));
        ui->orderItemIdlineEdit->setText(QString::number(i_id));
        ui->orderPricelineEdit->setText(QString::number(q));
        ui->orderQuantitylineEdit->setText(item->text(4));
        ui->orderAmountlineEdit->setText(item->text(5));
    }
}


void OrderManager::on_modifyPushButton_clicked()
{
    for (const auto& v : saveOrderList) {
        QTreeWidget* i = v;
        int key = saveOrderList.key(i);
//        qDebug() << saveOrderList[key]->text(1);
    }
}

