#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T16:51:12
#
#-------------------------------------------------

QT       += core gui network

TARGET = uvhCapture
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ethrelay.cpp \
    uvch264cam.cpp

HEADERS  += mainwindow.h \
    ethrelay.h \
    uvch264cam.h

FORMS    += mainwindow.ui

LIBS += /usr/lib64/libgobject-2.0.so.0 \
        /usr/lib64/libglib-2.0.so.0    \
         /usr/lib64/libgstreamer-0.10.so.0 \
#        /usr/lib64/libgstaudio-0.10.so \
        /home/peter/src/gstreamer-libs/lib/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so \
#        /home/peter/src/gstreamer-libs/lib/gst-plugins-good/gst/isomp4/.libs/libgstisomp4.so

unix {

    CONFIG += link_pkgconfig
    PKGCONFIG += QtGStreamer-0.10

}
