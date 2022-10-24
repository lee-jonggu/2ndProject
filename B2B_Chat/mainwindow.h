#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ChatMain;
class ClientChat;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openClientChat();

private:
    Ui::MainWindow *ui;
    ChatMain *chatMain;
    ClientChat *clientChat;
};

#endif // MAINWINDOW_H
