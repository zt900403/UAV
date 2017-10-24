#-------------------------------------------------
#
# Project created by QtCreator 2017-10-02T11:20:02
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UVAServer
TEMPLATE = app


SOURCES += main.cpp\
    components/rOg_image.cpp \
    mainwindow.cpp \
    utils/json.cpp \
    dialog/uavtypedatadialog.cpp \
    utils/filesystem.cpp \
    object/uav.cpp \
    object/weapon.cpp \
    net/uavtcpserver.cpp \
    net/uavtcpsocket.cpp \
    object/uavstatus.cpp

HEADERS  += \
    components/rOg_image.h \
    mainwindow.h \
    utils/json.h \
    dialog/uavtypedatadialog.h \
    utils/filesystem.h \
    object/uav.h \
    object/weapon.h \
    net/uavtcpserver.h \
    net/uavtcpsocket.h \
    object/uavstatus.h

FORMS    += \
    mainwindow.ui \
    dialog/uavtypedatadialog.ui

DISTFILES += \
    config/uvatype.json

RESOURCES += \
    qavserver.qrc
