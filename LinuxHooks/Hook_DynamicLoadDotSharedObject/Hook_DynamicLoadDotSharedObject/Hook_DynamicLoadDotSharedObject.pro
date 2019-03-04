#-------------------------------------------------
#
# Project created by QtCreator 2019-03-03T21:29:48
#
#-------------------------------------------------

QT       -= core gui

TEMPLATE = lib
TARGET = Hook_DynamicLoadDotSharedObject
DESTDIR = $$PWD/../bin

DEFINES += EIT_GIT_DEBUG

LIBS += -ldl

SOURCES += \
    Global.c \
    Hook_DynamicLoadDotSharedObject.cpp \
    Module.cpp \
    DlopenPtrAddr.c

HEADERS += Hook_DynamicLoadDotSharedObject.h \
    Global.h \
    Module.h \
    TypeDecl.h \
    DlopenPtrAddr.h
