#ifndef ORDERMANAGER_H
#define ORDERMANAGER_H

#include "client.h"
#include "item.h"
#include "ui_ordermanager.h"
#include <QWidget>
#include <QMap>

class Order;
class QTreeWidgetItem;
class ClientManager;
class ItemManager;

namespace Ui {
class OrderManager;
}

class OrderManager : public QWidget
{
    Q_OBJECT

public:
    explicit OrderManager(QWidget *parent = nullptr);
    ~OrderManager();

    void loadData();

private slots:
    void on_clientLineEdit_returnPressed();
    void showClientData(Client*);
    void showItemData(Item*);
//    void clientListData(QString);
//    void showItemNameData(Item*);
    void showClientNameData(Client*,QTreeWidgetItem*);
    void showItemNameData(Item*,QTreeWidgetItem*);

    void on_itemLineEdit_returnPressed();
    void on_clientTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_itemTreeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void on_orderQuantitylineEdit_returnPressed();
    void on_orderPushButton_clicked();

    void on_SearchPushButton_clicked();

    void on_clearPushButton_clicked();

    void on_orderTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_modifyPushButton_clicked();

signals:
    void clientDataSent(QString);
    void clientDataSent(int);

    void itemDataSent(QString);
    void itemDataSent(int);

    void clientNameDataSent(int,QTreeWidgetItem*);
    void itemNameDataSent(int,QTreeWidgetItem*);

private:
    Ui::OrderManager *ui;

    int makeId();
    QString date;

//    QMap<int, QTreeWidgetItem*> saveOrderList;
    QMap<int, QTreeWidget*> saveOrderList;
    QMap<int, Order*> orderList;
//    QTreeWidgetItem* row = new QTreeWidgetItem(ui->orderTreeWidget);
};

#endif // ORDERMANAGER_H
