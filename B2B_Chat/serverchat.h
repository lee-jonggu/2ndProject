#ifndef SERVERCHAT_H
#define SERVERCHAT_H

#include <QWidget>

namespace Ui {
class ServerChat;
}

class ServerChat : public QWidget
{
    Q_OBJECT

public:
    explicit ServerChat(QWidget *parent = nullptr);
    ~ServerChat();

private:
    Ui::ServerChat *ui;
};

#endif // SERVERCHAT_H
