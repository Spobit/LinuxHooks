#-------------------------------------------------
#
# Project created by QtCreator 2019-04-17T13:46:59
#
#-------------------------------------------------

QT       -= gui
CONFIG += debug

TEMPLATE = lib
TARGET = Hook

DEFINES += EIT_GIT_DEBUG

SOURCES += Hook.cpp

HEADERS +=

unix {
    target.path = /usr/lib
    INSTALLS += target
}
