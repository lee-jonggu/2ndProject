#include "order.h"

Order::Order(int orderNum, int c_id, int i_id, int count)
{
    setText(0, QString::number(orderNum));
    setText(1, QString::number(c_id));
    setText(2, QString::number(i_id));
    setText(3, QString::number(count));
}

int Order::orderNum() const
{
    return text(0).toInt();
}

int Order::getClientId() const
{
    return text(1).toInt();
}

void Order::setClientId(int& c_id)
{
    setText(1, QString::number(c_id));
}

int Order::getItemId() const
{
    return text(2).toInt();
}

void Order::setItemId(int& i_id)
{
    setText(2, QString::number(i_id));
}

int Order::getCount() const
{
    return text(3).toInt();
}

void Order::setCount(int& count)
{
    setText(3, QString::number(count));
}

bool Order::operator==(const Order& other) const
{
    return (this->text(1) == other.text(1));
}
