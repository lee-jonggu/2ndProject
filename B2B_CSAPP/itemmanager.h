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

    void loadData();                                        // 저장된 데이터 불러오기

public slots:
    void showItem(QTreeWidgetItem*,int);                    // 저장된 아이템 보여주기

    void itemNameListData(QString);                         // 이름으로 아이템 정보 추출
    void itemColorListData(QString);                        // 색상으로 아이템 정보 추출
    void itemCategoriListData(QString);                     // 카테고리로 아이템 정보 추출
    void itemIdListData(int);                               // ID로 아이템 정보 추출

    void itemIdNameListData(int,QTreeWidgetItem*);          // 아이템 ID, 이름으로 트리위젯아이템 형태로 저장

signals:
    void itemAdded(QString);                                // 새로운 아이템이 추가될 때 시그널

    void itemDataSent(Item*);                               // 아이템 정보 전송 시그널(아이템)
    void itemDataSent(QString);                             // 아이템 정보 전송 시그널(이름)
    void itemDataSent(int);                                 // 아이템 정보 전송 시그널(ID)

    void itemNameDataSent(Item*,QTreeWidgetItem*);          // 아이템이름으로 객체 전송 시그널

private slots:
    void on_AddPushButton_clicked();                        // add 버튼
    void on_ModifyPushButton_clicked();                     // modify 버튼
    void on_RemovePushButton_clicked();                     // remove 버튼
    void on_SearchPushButton_clicked();                     // search 버튼

private:
    Ui::ItemManager *ui;

    QMap<int, Item*> itemList;                              // id, 아이템 정보
    QMap<int, QString> addLogList;                          // id로 로그 리스트 관리
    QMultiMap<int, QString> logTimeList;                    // id로 로그 리스트 관리

    int makeId();                                           // 아이템 ID 생성

};

#endif // ITEMMANAGER_H
