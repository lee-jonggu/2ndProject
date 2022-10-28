#ifndef ITEM_H
#define ITEM_H

#include <QTreeWidgetItem>

class Item : public QTreeWidgetItem
{
public:

    explicit Item(int id=0, QString="", QString="", QString="", QString="", QString="");    // id,name,categori,color,stock,price

    int id() const;                                                                         // 아이템 id
    QString getName() const;                                                                // 아이템 이름 가져오기
    void setName(QString&);                                                                 // 아이템 이름 설정

    QString getCategori() const;                                                            // 아이템 카테고리 가져오기
    void setCategori(QString&);                                                             // 아이템 카테고리 설정

    QString getColor() const;                                                               // 아이템 색상 가져오기
    void setColor(QString&);                                                                // 아이템 색상 설정

    QString getStock() const;                                                               // 아이템 재고량 가져오기
    void setStock(QString&);                                                                // 아이템 재고량 설정

    QString getPrice() const;                                                               // 아이템 가격 가져오기
    void setPrice(QString&);                                                                // 아이템 가격 설정

    bool operator==(const Item &other) const;                                               // 아이템 간 비교
};

#endif // ITEM_H
