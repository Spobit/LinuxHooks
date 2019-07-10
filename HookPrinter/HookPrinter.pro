#-------------------------------------------------
#
# Project created by QtCreator 2019-07-07T02:45:41
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
TARGET = HookPrinter

DEFINES += _IPP_PRIVATE_STRUCTURES EIT_GIT_DEBUG
# DEFINES += _HAVE_MULTI_THREAD_SAFETY_FLAG

LIBS += -lcups

SOURCES += CupsHook.cpp \
    Type.cpp \
    Global.cpp \
    DlsymHook.cpp \
    WhiteList.cpp

HEADERS += CupsHook.h \
    Type.h \
    Global.h \
    DlsymHook.h \
    WhiteList.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
