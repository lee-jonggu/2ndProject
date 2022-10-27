#include "client.h"

Client::Client(int id, QString name, QString address, QString phoneNum, QString type)   // 디폴트 매개 변수
{
    setText(0, QString::number(id));        // 클라이언트 id 이니셜라이저
    setText(1, name);                       // 클라이언트 이름 이니셜라이저
    setText(2, address);                    // 클라이언트 주소 이니셜라이저
    setText(3, phoneNum);                   // 클라이언트 전화번호 이니셜라이저
    setText(4, type);                       // 클라이언트 타입 이니셜라이저
}

int Client::id() const
{
    return text(0).toInt();                 // 클라이언트 id
}

QString Client::getName() const             // 클라이언트 이름 가져오기
{
    return text(1);
}

void Client::setName(QString& name)         // 클라이언트 이름 설정
{
    setText(1, name);
}

QString Client::getAddress() const          // 클라이언트 주소 가져오기
{
    return text(2);
}

void Client::setAddress(QString& address)   // 클라이언트 주소 설정
{
    setText(2, address);
}

QString Client::getPhoneNum() const         // 클라이언트 전화번호 가져오기
{
    return text(3);
}

void Client::setPhoneNum(QString& phoneNum) // 클라이언트 전화번호 설정
{
    setText(3, phoneNum);
}

QString Client::getType() const             // 클라이언트 타입 가져오기
{
    return text(4);
}

void Client::setType(QString& type)         // 클라이언트 타입 설정
{
    setText(4, type);
}

bool Client::operator==(const Client& other) const  // 클라이언트 간 비교 연산자 오버로딩
{
    return (this->text(1) == other.text(1));
}
