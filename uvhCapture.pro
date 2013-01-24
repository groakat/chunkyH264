#-------------------------------------------------
#
# Project created by QtCreator 2012-11-06T16:51:12
#
#-------------------------------------------------

QT       += core thread gui network

TARGET = uvhCapture
TEMPLATE = app

#DEFINES     += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    ethrelay.cpp \
    uvch264cam.cpp

HEADERS  += mainwindow.h \
    ethrelay.h \
    uvch264cam.h

FORMS    += mainwindow.ui

INCLUDEPATH += /home/peter/src/gstreamer-libs/lib/gstreamer/libs/gst/base/ \
               /home/peter/src/gstreamer-libs/lib/gstreamer/libs/gst/gst-plugins-base/gst-libs/gst/video/ \
               /home/peter/src/gstreamer-libs/lib/gstreamer/libs/gst/base/



LIBS += /usr/lib64/libgobject-2.0.so.0 \
        /usr/lib64/libglib-2.0.so.0    \
         /usr/lib64/libgstreamer-0.10.so.0 \
\
#         /home/peter/src/gstreamer-libs/lib/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so.0 \
#         /home/peter/src/gstreamer-libs/lib/gstreamer/gst/.libs/libgstreamer-0.10.so.0   \
#         /usr/lib64/libgstvideo-0.10.so.0 \
#        /usr/lib64/libgstaudio-0.10.so \
#        ~/src/gstreamer-head/head/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so \
#        ~/src/gstreamer-head/head/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so
#        /home/peter/src/gstreamer-libs/lib/gst-plugins-good/gst/isomp4/.libs/libgstisomp4.so \
#            /home/peter/src/gstreamer-libs/lib/gstreamer/libs/gst/base/.libs/libgstbase-0.10.so \
#             ~/src/gstreamer/head/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so \
#             ~/src/gstreamer/head/gst-plugins-base/gst-libs/gst/video/.libs/libgstvideo-0.10.so \

unix {

    CONFIG += link_pkgconfig
    PKGCONFIG += QtGStreamer-0.10
}
