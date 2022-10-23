QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatserver.cpp \
    chatwindow.cpp \
    client.cpp \
    clientmanager.cpp \
    item.cpp \
    itemmanager.cpp \
    logthread.cpp \
    main.cpp \
    mainwindow.cpp \
    order.cpp \
    ordermanager.cpp

HEADERS += \
    chatserver.h \
    chatwindow.h \
    client.h \
    clientmanager.h \
    item.h \
    itemmanager.h \
    logthread.h \
    mainwindow.h \
    order.h \
    ordermanager.h

FORMS += \
    chatserver.ui \
    chatwindow.ui \
    clientmanager.ui \
    itemmanager.ui \
    mainwindow.ui \
    ordermanager.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
