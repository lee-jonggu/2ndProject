#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>
#include <QList>

class QTreeWidgetItem;

class LogThread : public QThread
{
    Q_OBJECT
public:
    explicit LogThread(QObject *parent = nullptr);

private:
    void run();                                             // 로그 스레드 시작

    QList<QTreeWidgetItem*> itemList;                       // 로그 파일에 올릴 트리위젯 아이템
    QString filename;                                       // 로그 파일 저장 경로

signals:
    void send(int data);                                    // 정보 전송 시그널

public slots:
    void appendData(QTreeWidgetItem*);                      // itemList에 로그 올리기
    void saveData();                                        // 로그 파일 저장
};

#endif // LOGTHREAD_H
