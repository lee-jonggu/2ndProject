#include "chatmain.h"
#include "clientchat.h"
#include "ui_chatmain.h"

#include <QWidget>
//#include <QString>

ChatMain::ChatMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatMain)
{
    ui->setupUi(this);
}

ChatMain::~ChatMain()
{
    delete ui;
}

void ChatMain::on_pushButton_clicked()
{
    int id = ui->lineEdit->text().toInt();
    emit mainToClient(id);
    emit clickBtn();
}

