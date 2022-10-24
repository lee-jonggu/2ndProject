#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTcpSocket>

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();

private slots:
    void echoData();
    void sendData();

    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::ChatWindow *ui;

    QTextEdit *message;
    QLineEdit *inputLine;
    QTcpSocket *clientSocket;
    QList<QTcpSocket*> clientList;
};

#endif // CHATWINDOW_H
