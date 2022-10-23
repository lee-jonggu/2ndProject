#include "clientchat.h"
#include "chatmain.h"
#include "mainwindow.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ClientChat c;
    c.resize(500,500);
    c.show();

    return a.exec();
}
