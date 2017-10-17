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
        maindialog.cpp \
    components/rOg_image.cpp

HEADERS  += maindialog.h \
    components/rOg_image.h

FORMS    += maindialog.ui
