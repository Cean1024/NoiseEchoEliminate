TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./include ./webrtc
LIBS += -lasound  -lspeexdsp -lPocoFoundation -lpthread -lopus -L$$PWD/libs -lwiringPi

SOURCES += main.cpp \
    audio/alsahandle.cpp \
    speexhandle/speexhandler.cpp \
    linklist.cpp \
    opushandle.cpp \
    mfcc/mfcc.cpp \
    mfcc/mfcchandle.cpp \
    gpiocontrol.cpp

HEADERS += \
    audio/alsahandle.h \
    speexhandle/speexhandler.h \
    linklist.h \
    include/common.h \
    opushandle.h \
    mfcc/mfcchandle.h \
    gpiocontrol.h

