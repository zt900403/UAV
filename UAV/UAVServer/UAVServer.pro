#-------------------------------------------------
#
# Project created by QtCreator 2017-10-02T11:20:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UVAServer
TEMPLATE = app


SOURCES += main.cpp\
    components/rOg_image.cpp \
    mainwindow.cpp \
    utils/json.cpp

HEADERS  += \
    components/rOg_image.h \
    mainwindow.h \
    utils/json.h

FORMS    += \
    mainwindow.ui
