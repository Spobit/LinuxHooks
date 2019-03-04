QT += core
QT -= gui

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = Test
DESTDIR = $$PWD/../bin

LIBS += -ldl

SOURCES += main.cpp

