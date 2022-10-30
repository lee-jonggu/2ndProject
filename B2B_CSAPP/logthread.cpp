#include "logthread.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QDateTime>

LogThread::LogThread(QObject *parent)
    : QThread{parent}
{
    QString format = "yyyyMMdd_hhmmss";                                                     // 로그 저장 형태
    filename = QString("log_%1.txt").arg(QDateTime::currentDateTime().toString(format));    // 로그 저장 파일 이름
}

void LogThread::run()                                                                       // 로그 스레드 실행
{
    Q_FOREVER {                                                                             // 로그는 멈추지 않고 저장
        saveData();                                                                         // 로그 저장
        sleep(60);                                                                          // 1분마다 저장
    }
}

void LogThread::appendData(QTreeWidgetItem* item)                                           // 로그 데이터 추가
{
    itemList.append(item);                                                                  // 로그 리스트에 추가
}

void LogThread::saveData()                                                                  // 로그 저장
{
    if(itemList.count() > 0) {                                                              // 로그가 있다면
        QFile file(filename);                                                               // 파일 열기
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))                             // 읽기 전용, 텍스트
            return;

        QTextStream out(&file);                                                             // 저장할 텍스트 스트림
        foreach(auto item, itemList) {                                                      // 저장된 로그를 돌면서
            out << item->text(0) << ", ";                                                   // 로그 저장
            out << item->text(1) << ", ";                                                   // 로그 저장
            out << item->text(2) << ", ";                                                   // 로그 저장
            out << item->text(3) << ", ";                                                   // 로그 저장
            out << item->text(4) << ", ";                                                   // 로그 저장
            out << item->text(5) << "\n";                                                   // 로그 저장
        }
        file.close();                                                                       // 파일 저장 후 닫기
    }
}
