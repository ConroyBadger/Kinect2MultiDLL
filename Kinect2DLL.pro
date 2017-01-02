#-------------------------------------------------
#
# Project created by QtCreator 2015-08-04T20:51:16
#
#-------------------------------------------------

QT       -= gui

TARGET = Kinect2DLL
TEMPLATE = lib

DEFINES += KINECT2DLL_LIBRARY

SOURCES += kinect2dll.cpp kinect2.cpp

HEADERS += kinect2dll.h \
           kinect2dll_global.h \
           kinect2.h \
           Kinect.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}

#CONFIG(release, debug|release): LIBS += -L$$PWD/-lKinect20


win32: LIBS += -L$$PWD/./ -lKinect20

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
