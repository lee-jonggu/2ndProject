#ifndef CLIENT_H
#define CLIENT_H

#include <QTreeWidgetItem>

class Client : public QTreeWidgetItem
{
public:
    explicit Client(int id=0, QString="", QString="", QString="", QString="");  // id,name,address,phone num, type

    int id() const;
    QString getName() const;
    void setName(QString&);

    QString getAddress() const;
    void setAddress(QString&);

    QString getPhoneNum() const;
    void setPhoneNum(QString&);

    QString getType() const;
    void setType(QString&);

    bool operator==(const Client &other) const;
};

#endif // CLIENT_H
