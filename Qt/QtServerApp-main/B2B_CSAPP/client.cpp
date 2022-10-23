#include "client.h"

Client::Client(int id, QString name, QString address, QString phoneNum, QString type)
{
    setText(0, QString::number(id));
    setText(1, name);
    setText(2, address);
    setText(3, phoneNum);
    setText(4, type);
}

int Client::id() const
{
    return text(0).toInt();
}

QString Client::getName() const
{
    return text(1);
}

void Client::setName(QString& name)
{
    setText(1, name);
}

QString Client::getAddress() const
{
    return text(2);
}

void Client::setAddress(QString& address)
{
    setText(2, address);
}

QString Client::getPhoneNum() const
{
    return text(3);
}

void Client::setPhoneNum(QString& phoneNum)
{
    setText(3, phoneNum);
}

QString Client::getType() const
{
    return text(4);
}

void Client::setType(QString& type)
{
    setText(4, type);
}

bool Client::operator==(const Client& other) const
{
    return (this->text(1) == other.text(1));
}

