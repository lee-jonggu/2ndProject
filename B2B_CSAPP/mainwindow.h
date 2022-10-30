#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ClientManager;
class ItemManager;
class OrderManager;
class ChatServer;
class ChatWindow;
class ServerClientChat;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();                   // Chat 버튼

signals:
    void makeServerTab();                           // 새로운 탭 생성

private:
    Ui::MainWindow *ui;
    ClientManager *clientManager;                   // ClientManager 객체 생성
    ItemManager *itemManager;                       // ItemManager 객체 생성
    OrderManager *orderManager;                     // OrderManager 객체 생성
    ChatServer *chatManager;                        // ChatManager 객체 생성
    ChatWindow *chatWindow;                         // ChatWindow 객체 생성
    ServerClientChat *serverClientChat;             // serverClientChat 객체 생성
};
#endif // MAINWINDOW_H
