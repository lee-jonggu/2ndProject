#include "order.h"

Order::Order(int orderNum, int c_id, int i_id, int count)       // orderNum, client ID, item ID, count
{
    setText(0, QString::number(orderNum));                      // 주문 번호
    setText(1, QString::number(c_id));                          // 클라이언트 아이디
    setText(2, QString::number(i_id));                          // 아이템 아이디
    setText(3, QString::number(count));                         // 주문 수량
}

int Order::orderNum() const
{
    return text(0).toInt();                                     // 주문 번호
}

int Order::getClientId() const                                  // 클라이언트 아이디 가져오기
{
    return text(1).toInt();
}

void Order::setClientId(int& c_id)                              // 클라이언트 아이디 설정
{
    setText(1, QString::number(c_id));
}

int Order::getItemId() const                                    // 아이템 아이디 가져오기
{
    return text(2).toInt();
}

void Order::setItemId(int& i_id)                                // 아이템 아이디 설정
{
    setText(2, QString::number(i_id));
}

int Order::getCount() const                                     // 주문 수량 가져오기
{
    return text(3).toInt();
}

void Order::setCount(int& count)                                // 주문 수량 설정
{
    setText(3, QString::number(count));
}

bool Order::operator==(const Order& other) const                // 주문 번호 간 비교 연산자 오버로딩
{
    return (this->text(1) == other.text(1));
}
