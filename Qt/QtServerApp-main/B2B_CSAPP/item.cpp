#include "item.h"

Item::Item(int id, QString name, QString categori, QString color, QString stock, QString price)
{
    setText(0, QString::number(id));
    setText(1, name);
    setText(2, categori);
    setText(3, color);
    setText(4, stock);
    setText(5, price);
}

int Item::id() const
{
    return text(0).toInt();
}

QString Item::getName() const
{
    return text(1);
}

void Item::setName(QString& name)
{
    setText(1, name);
}

QString Item::getCategori() const
{
    return text(2);
}

void Item::setCategori(QString& categori)
{
    setText(2, categori);
}

QString Item::getColor() const
{
    return text(3);
}

void Item::setColor(QString& color)
{
    setText(3, color);
}

QString Item::getStock() const
{
    return text(4);
}

void Item::setStock(QString& stock)
{
    setText(4, stock);
}

QString Item::getPrice() const
{
    return text(5);
}

void Item::setPrice(QString& price)
{
    setText(4, price);
}

bool Item::operator==(const Item& other) const
{
    return (this->text(1) == other.text(1));
}
