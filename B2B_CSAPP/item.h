#ifndef ITEM_H
#define ITEM_H

#include <QTreeWidgetItem>

class Item : public QTreeWidgetItem
{
public:

    explicit Item(int id=0, QString="", QString="", QString="", QString="", QString="");
    // id,name,categori,color,stock,price

    int id() const;
    QString getName() const;
    void setName(QString&);

    QString getCategori() const;
    void setCategori(QString&);

    QString getColor() const;
    void setColor(QString&);

    QString getStock() const;
    void setStock(QString&);

    QString getPrice() const;
    void setPrice(QString&);

    bool operator==(const Item &other) const;
};

#endif // ITEM_H
