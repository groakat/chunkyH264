#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T16:51:12
#
#-------------------------------------------------

QT       += core gui

TARGET = uvhCapture
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += /usr/lib64/libgobject-2.0.so.0 \
        /usr/lib64/libglib-2.0.so.0    \
         /usr/lib64/libgstreamer-0.10.so.0

unix {

    CONFIG += link_pkgconfig
    PKGCONFIG += QtGStreamer-0.10

}
