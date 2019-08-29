#-------------------------------------------------
#
# Project created by QtCreator 2019-07-07T02:45:41
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
TARGET = HookPrinter

DEFINES += _IPP_PRIVATE_STRUCTURES EIT_GIT_DEBUG
# OFFSET FROM DLERROR TO DLSYM
DEFINES += _OFFSET_FROM_DLERROR_TO_DLSYM=0x280
message("111 $${_OFFSET_FROM_DLERROR_TO_DLSYM}")
#error("if _OFFSET_FROM_DLERROR_TO_DLSYM is no problem, please comment this")

LIBS += -lcups

SOURCES += CupsHook.cpp \
    Type.cpp \
    Global.cpp \
    DlsymHook.cpp \
    WhiteList.cpp \
    CupsFullNameHook.cpp

HEADERS += CupsHook.h \
    Type.h \
    Global.h \
    DlsymHook.h \
    WhiteList.h \
    CupsFullNameHook.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    README.md
