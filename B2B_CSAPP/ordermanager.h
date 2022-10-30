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

    void loadData();                                        // 저장된 데이터 불러오기

private slots:
    void on_clientLineEdit_returnPressed();                 // 입력창에서 엔터키
    void showClientData(Client*);                           // 저장된 고객사 보여주기
    void showItemData(Item*);                               // 저장된 아이템 보여주기
    void showClientNameData(Client*,QTreeWidgetItem*);      // 고객사 이름으로 고객사 정보 추출
    void showItemNameData(Item*,QTreeWidgetItem*);          // 아이템 이름으로 아이템 정보 추출

    void on_itemLineEdit_returnPressed();                   // 입력창에서 엔터키
    void on_clientTreeWidget_itemClicked(QTreeWidgetItem *item, int column);    // 고객사 이름 클릭 시 입력창에 고객사 정보 보여주기
    void on_itemTreeWidget_itemClicked(QTreeWidgetItem *item, int column);      // 아이템 이름 클릭 시 입력창에 아이템 정보 보여주기
    void on_orderQuantitylineEdit_returnPressed();          // 아이템 수량 입력 후 엔터
    void on_orderPushButton_clicked();                      // order 버튼
    void on_SearchPushButton_clicked();                     // seach 버튼
    void on_clearPushButton_clicked();                      // clear 버튼
    void on_orderTreeWidget_itemClicked(QTreeWidgetItem *item, int column);     // 주문 정보 클릭 시 입력창에 주문 정보 보여주기
    void on_RemovePushButton_clicked();                     // remove 버튼

signals:
    void clientDataSent(QString);                           // 고객사 이름으로 된 정보 시그널
    void clientDataSent(int);                               // 고객사 ID로 된 정보 시그널

    void itemDataSent(QString);                             // 아이템 이름으로 된 정보 시그널
    void itemDataSent(int);                                 // 아이템 ID로 된 정보 시그널

    void clientNameDataSent(int,QTreeWidgetItem*);          // 고객사 ID로 트리위젯아이템 전송 시그널
    void itemNameDataSent(int,QTreeWidgetItem*);            // 아이템 ID로 트리위엣아이템 전송 시그널

private:
    Ui::OrderManager *ui;

    int makeId();                                           // 주문 번호 생성
    QString date;                                           // 주문 날짜를 위한 날짜 변수
    QMap<int, QTreeWidget*> saveOrderList;                  // 주문 정보 저장 리스트
    QMap<int, Order*> orderList;                            // 주문 리스트
};

#endif // ORDERMANAGER_H
