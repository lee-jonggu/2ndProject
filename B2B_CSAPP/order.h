#ifndef ORDER_H
#define ORDER_H

#include <QTreeWidgetItem>

class Order : public QTreeWidgetItem
{
public:
    explicit Order(int orderNum=0, int c_id=0, int i_id=0, int count=0);

    int orderNum() const;                           // 주문 번호

    int getClientId() const;                        // 주문 번호 가져오기
    void setClientId(int&);                         // 주문 번호 설정

    int getItemId() const;                          // 주문한 고객사 ID 가져오기
    void setItemId(int&);                           // 주문한 고객사 ID 설정

    int getCount() const;                           // 주문한 아이템 수량 가져오기
    void setCount(int&);                            // 주문한 아이템 수량 설정

    bool operator==(const Order &other) const;      // 주문 비교 연산자 오버로딩
};

#endif // ORDER_H
