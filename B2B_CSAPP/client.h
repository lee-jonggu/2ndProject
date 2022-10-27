#ifndef CLIENT_H
#define CLIENT_H

#include <QTreeWidgetItem>

class Client : public QTreeWidgetItem
{
public:
    explicit Client(int id=0, QString="", QString="", QString="", QString="");  // id,name,address,phone num, type

    int id() const;                                 // 클라이언트 id
    QString getName() const;                        // 클라이언트 이름 가져오기
    void setName(QString&);                         // 클라이언트 이름 설정

    QString getAddress() const;                     // 클라이언트 주소 가져오기
    void setAddress(QString&);                      // 클라이언트 주소 설정

    QString getPhoneNum() const;                    // 클라이언트 전화번호 가져오기
    void setPhoneNum(QString&);                     // 클라이언트 전화번호 설정

    QString getType() const;                        // 클라이언트 타입 가져오기
    void setType(QString&);                         // 클라이언트 타입 설정

    bool operator==(const Client &other) const;     // 클라이언트 비교 연산자 오버로딩
};

#endif // CLIENT_H
