#ifndef ITEMMANAGER_H
#define ITEMMANAGER_H

#include <QWidget>
#include <QHash>

class Item;
class QTreeWidgetItem;

namespace Ui {
class ItemManager;
}

class ItemManager : public QWidget
{
    Q_OBJECT

public:
    explicit ItemManager(QWidget *parent = nullptr);
    ~ItemManager();

    void loadData();

public slots:
    void showItem(QTreeWidgetItem*,int);

    void itemNameListData(QString);
    void itemColorListData(QString);
    void itemCategoriListData(QString);
    void itemIdListData(int);

    void itemIdNameListData(int,QTreeWidgetItem*);

signals:
    void itemAdded(QString);

    void itemDataSent(Item*);
    void itemDataSent(QString);
    void itemDataSent(int);

    void itemNameDataSent(Item*,QTreeWidgetItem*);

private slots:
    void on_AddPushButton_clicked();
    void on_ModifyPushButton_clicked();
    void on_RemovePushButton_clicked();
    void on_SearchPushButton_clicked();

//    void on_ItemTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::ItemManager *ui;

    QMap<int, Item*> itemList;

    QMap<int, QString> addLogList;
    QMultiMap<int, QString> logTimeList;

    int makeId();

};

#endif // ITEMMANAGER_H
