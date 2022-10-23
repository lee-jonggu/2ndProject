#ifndef ORDER_H
#define ORDER_H

#include <QTreeWidgetItem>

class Order : public QTreeWidgetItem
{
public:
    explicit Order(int orderNum=0, int c_id=0, int i_id=0, int count=0);

    int orderNum() const;

    int getClientId() const;
    void setClientId(int&);

    int getItemId() const;
    void setItemId(int&);

    int getCount() const;
    void setCount(int&);

    bool operator==(const Order &other) const;
};

#endif // ORDER_H
