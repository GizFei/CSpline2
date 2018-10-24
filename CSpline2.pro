#-------------------------------------------------
#
# Project created by QtCreator 2018-09-20T18:21:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CSpline2
TEMPLATE = app


SOURCES += main.cpp\
        mywidget.cpp \
    paintwindow.cpp \
    cspline.cpp

HEADERS  += mywidget.h \
    cspline.h \
    paintwindow.h

FORMS    += mywidget.ui

RESOURCES += \
    qss.qrc \
    icons.qrc
