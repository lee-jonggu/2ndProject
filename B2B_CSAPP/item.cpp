#include "item.h"

Item::Item(int id, QString name, QString categori, QString color, QString stock, QString price) // 디폴트 매개 변수
{
    setText(0, QString::number(id));            // 아이템 id 이니셜라이저
    setText(1, name);                           // 아이템 이름 이니셜라이저
    setText(2, categori);                       // 아이템 카테고리
    setText(3, color);                          // 아이템 색상
    setText(4, stock);                          // 아이템 재고량
    setText(5, price);                          // 아이템 가격
}

int Item::id() const
{
    return text(0).toInt();                     // 아이템 id
}

QString Item::getName() const
{
    return text(1);                             // 아이템 이름 가져오기
}

void Item::setName(QString& name)
{
    setText(1, name);                           // 아이템 이름 설정
}

QString Item::getCategori() const
{
    return text(2);                             // 아이템 카테고리 가져오기
}

void Item::setCategori(QString& categori)       // 아이템 설정 가져오기
{
    setText(2, categori);
}

QString Item::getColor() const
{
    return text(3);                             // 아이템 색상 가져오기
}

void Item::setColor(QString& color)
{
    setText(3, color);                          // 아이템 색상 설정
}

QString Item::getStock() const
{
    return text(4);                             // 아이템 재고량 가져오기
}

void Item::setStock(QString& stock)
{
    setText(4, stock);                          // 아이템 재고량 설정
}

QString Item::getPrice() const
{
    return text(5);                             // 아이템 가격 가져오기
}

void Item::setPrice(QString& price)
{
    setText(5, price);                          // 아이템 가격 설정
}

bool Item::operator==(const Item& other) const
{
    return (this->text(1) == other.text(1));    // 아이템 간 비교 연산자 오버로딩
}
