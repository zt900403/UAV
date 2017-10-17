#-------------------------------------------------
#
# Project created by QtCreator 2017-09-28T20:50:28
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UAVClient
TEMPLATE = app


SOURCES += main.cpp\
        maindialog.cpp \
    components/qfi_PFD.cpp \
    widgets/LayoutSquare.cpp \
    widgets/WidgetPFD.cpp \
    components/switchbutton.cpp \
    components/aircraftcontroller.cpp

HEADERS  += maindialog.h \
    components/qfi_PFD.h \
    widgets/LayoutSquare.h \
    widgets/WidgetPFD.h \
    components/switchbutton.h \
    components/aircraftcontroller.h

FORMS    += maindialog.ui \
    widgets/WidgetPFD.ui

DISTFILES += \
    images/pfd/pfd.svg \
    images/pfd/pfd_adi_back.svg \
    images/pfd/pfd_adi_barh.svg \
    images/pfd/pfd_adi_barv.svg \
    images/pfd/pfd_adi_doth.svg \
    images/pfd/pfd_adi_dotv.svg \
    images/pfd/pfd_adi_ladd.svg \
    images/pfd/pfd_adi_mark.svg \
    images/pfd/pfd_adi_mask.svg \
    images/pfd/pfd_adi_path.svg \
    images/pfd/pfd_adi_roll.svg \
    images/pfd/pfd_adi_scaleh.svg \
    images/pfd/pfd_adi_scalev.svg \
    images/pfd/pfd_adi_slip.svg \
    images/pfd/pfd_adi_turn.svg \
    images/pfd/pfd_alt_back.svg \
    images/pfd/pfd_alt_frame.svg \
    images/pfd/pfd_alt_ground.svg \
    images/pfd/pfd_alt_scale.svg \
    images/pfd/pfd_asi_back.svg \
    images/pfd/pfd_asi_frame.svg \
    images/pfd/pfd_asi_scale.svg \
    images/pfd/pfd_back.svg \
    images/pfd/pfd_hsi_back.svg \
    images/pfd/pfd_hsi_face.svg \
    images/pfd/pfd_hsi_marks.svg \
    images/pfd/pfd_mask.svg \
    images/pfd/pfd_vsi_arrow.svg \
    images/pfd/pfd_vsi_scale.svg

RESOURCES += \
    uva.qrc
