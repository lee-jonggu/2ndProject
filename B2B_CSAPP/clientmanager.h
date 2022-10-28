#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QWidget>
#include <QHash>

class Client;
class QMenu;
class QTreeWidgetItem;

namespace Ui {
class ClientManager;
}

class ClientManager : public QWidget
{
    Q_OBJECT

public:
    explicit ClientManager(QWidget *parent = nullptr);
    ~ClientManager();
    void loadData();

public slots:
    void showClient(QTreeWidgetItem*,int);                  // 저장된 클라이언트 보여주기

    void clientNameListData(QString);                       // 이름으로 클라이언트 정보 추출
    void clientAddressListData(QString);                    // 주소로 클라이언트 정보 추출
    void clientTypeListData(QString);                       // 타입으로 클라이언트 정보 추출
    void clientIdListData(int);                             // 클라이언트 id로 서버에 정보 전송

    void clientIdNameListData(int,QTreeWidgetItem*);        // 클라이언트 id, 이름으로 트리위젯아이템 형태로 저장

    void serverClientList();                                // 서버에 접속한 클라이언트 관리

signals:
    void clientAdded(int,QString);                          // 새로운 클라이언트가 추가될 때 시그널
    void clientRemove(int,int);                             // 클라이언트가 삭제될 때 시그널
    void clientDataSent(Client*);                           // 클라이언트 정보 전송 시그널(클라이언트)
    void clientDataSent(QString);                           // 클라이언트 정보 전송 시그널(이름)
    void clientDataSent(int);                               // 클라이언트 정보 전송 시그널(id)
    void clientNameDataSent(Client*,QTreeWidgetItem*);      // 클라이언트 이름으로 객체 전송 시그널

    void clientToServer(QTreeWidgetItem*);                  // 서버로 클라이언트 정보 전송 시그널

    void clickedServerTabSignal();                          // 탭이 바뀔 때 시그널 전송

private slots:
    void on_ClientInfoAddPushButton_clicked();              // add 버튼
    void on_ClientInfoModifyPushButton_clicked();           // modify 버튼
    void on_ClientInfoRemovePushButton_clicked();           // remove 버튼
    void on_ClientSearchPushButton_clicked();               // search 버튼

private:
    Ui::ClientManager *ui;

    int makeId();                                           // 클라이언트 id 생성

    QMap<int, Client*> clientList;                          // id, 클라이언트 정보
    QMap<int, QString> addLogList;                          // id로 로그 리스트 관리
    QMultiMap<int, QString> logTimeList;                    // id로 로그 리스트 관리
    QMenu* menu;                                            // 메뉴
    QMap<int, QString> serverClient;                        // id로 서버에 전송하는 클라이언트 리스트
};

#endif // CLIENTMANAGER_H
