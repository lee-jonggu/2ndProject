#ifndef SERVERCLIENTCHAT_H
#define SERVERCLIENTCHAT_H

#include "chatserver.h"

#include <QWidget>

namespace Ui {
class ServerClientChat;
}

class ServerClientChat : public QWidget
{
    Q_OBJECT

public:
    explicit ServerClientChat(int = 0, QString = "", QWidget *parent = nullptr);
    ~ServerClientChat();

signals:
    void sendData(int,QString);

private slots:
    void on_sendPushButton_clicked();

    void receiveData();

private:
    Ui::ServerClientChat *ui;

    int c_id;
    QString c_name;
};

#endif // SERVERCLIENTCHAT_H
