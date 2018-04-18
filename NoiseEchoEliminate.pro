TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./include ./webrtc
LIBS += -lasound -lspeex -lspeexdsp -lPocoFoundation -lpthread

SOURCES += main.cpp \
    audio/alsahandle.cpp \
    speex/speexhandler.cpp \
    linklist.cpp

HEADERS += \
    audio/alsahandle.h \
    speex/speexhandler.h \
    linklist.h \
    include/common.h

