#ifndef CHATMAIN_H
#define CHATMAIN_H

#include <QWidget>

namespace Ui {
class ChatMain;
}

class ChatMain : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMain(QWidget *parent = nullptr);
    ~ChatMain();

private slots:
    void on_pushButton_clicked();

signals:
    void clickBtn();
    void mainToClient(int);

private:
    Ui::ChatMain *ui;
};

#endif // CHATMAIN_H
