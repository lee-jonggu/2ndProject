#include "serverchat.h"
#include "ui_serverchat.h"

ServerChat::ServerChat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerChat)
{
    ui->setupUi(this);
}

ServerChat::~ServerChat()
{
    delete ui;
}
