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
    void showClient(QTreeWidgetItem*,int);

    void clientNameListData(QString);
    void clientAddressListData(QString);
    void clientTypeListData(QString);
    void clientIdListData(int);

    void clientIdNameListData(int,QTreeWidgetItem*);

    void serverClientList();

signals:
    void clientAdded(int,QString);
    void clientRemove(int,int);
    void clientDataSent(Client*);
    void clientDataSent(QString);
    void clientDataSent(int);
    void clientNameDataSent(Client*,QTreeWidgetItem*);

    void clientToServer(QTreeWidgetItem*);

    void clickedServerTabSignal();

private slots:
    // QTreeWidget 슬롯
//    void on_ClientSearchInfoTreeWidget_itemClicked(QTreeWidgetItem *item, int column);

    // QAction 슬롯
    void on_ClientInfoAddPushButton_clicked();
    void on_ClientInfoModifyPushButton_clicked();
    void on_ClientInfoRemovePushButton_clicked();

    void on_ClientSearchPushButton_clicked();

private:
    Ui::ClientManager *ui;

    int makeId();

    QMap<int, Client*> clientList;

    QMap<int, QString> addLogList;
    QMultiMap<int, QString> logTimeList;
    QMenu* menu;

    QMap<int, QString> serverClient;
};

#endif // CLIENTMANAGER_H
