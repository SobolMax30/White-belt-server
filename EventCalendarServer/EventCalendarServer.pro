QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    eventstorage.cpp \
    main.cpp \
    mainwindow.cpp \
    network.cpp \
    packetreceiver.cpp \
    packetsender.cpp

HEADERS += \
    eventstorage.h \
    mainwindow.h \
    network.h \
    packetreceiver.h \
    packetsender.h

FORMS += \
    mainwindow.ui
