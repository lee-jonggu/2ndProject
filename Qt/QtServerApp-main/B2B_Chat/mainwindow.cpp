#include "mainwindow.h"
#include "chatmain.h"
#include "clientchat.h"
#include "ui_mainwindow.h"
#include <QMdiArea>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    clientChat = new ClientChat(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openClientChat()
{
    clientChat->setFocus();
}
