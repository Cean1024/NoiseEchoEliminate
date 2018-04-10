TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./include
LIBS += -lasound

SOURCES += main.cpp \
    audio/alsahandle.cpp

HEADERS += \
    audio/alsahandle.h
