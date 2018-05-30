TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ./include ./webrtc /usr/local/include/sphinxbase /usr/local/include/pocketsphinx
LIBS += -lasound  -lspeexdsp -lPocoFoundation -lpthread -lopus  -lwiringPi \
        -lpocketsphinx -lsphinxbase -lsphinxad

SOURCES += main.cpp \
    audio/alsahandle.cpp \
    speexhandle/speexhandler.cpp \
    linklist.cpp \
    opushandle.cpp \
    mfcc/mfcchandle.cpp \
    gpiocontrol.cpp \
    keyword/keyworddetect.cpp \
    keyword/pocketsphinxkeyword.cpp \
    network/netclient.cpp \
    kwdethandler.cpp

HEADERS += \
    audio/alsahandle.h \
    speexhandle/speexhandler.h \
    linklist.h \
    include/common.h \
    opushandle.h \
    mfcc/mfcchandle.h \
    gpiocontrol.h \
    keyword/keyworddetect.h \
    keyword/pocketsphinxkeyword.h \
    network/netclient.h \
    kwdethandler.h

