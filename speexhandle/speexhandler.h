#ifndef SPEEXHANDLER_H
#define SPEEXHANDLER_H
#include <iostream>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>
#include "common.h"

#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"
#include "audio/alsahandle.h"

#define DENOISE_DB (20)

class SpeexHandler:public Poco::Thread, public Poco::Runnable
{
public:
    SpeexHandler(int frames, int samplerate);
    virtual ~SpeexHandler();

    int audioprocess(char *buf);
    void addlist(Linklist &list);
protected:

    void run();
private:
    SpeexPreprocessState  *preprocess_state;
    SpeexEchoState *echo_state;
    Linklist *datalist;
    AlsaHandle *reader;
};

#endif // SPEEXHANDLER_H
