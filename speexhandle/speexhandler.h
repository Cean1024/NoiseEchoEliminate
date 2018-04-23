#ifndef SPEEXHANDLER_H
#define SPEEXHANDLER_H
#include <iostream>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>
#include "common.h"

#define DENOISE_DB (20)

class SpeexHandler
{
public:
    SpeexHandler(int frames, int samplerate);
    virtual ~SpeexHandler();

    int audioprocess(char *buf)
    {
        return speex_preprocess_run(preprocess_state,(spx_int16_t *)buf);
    }

private:
    SpeexPreprocessState  *preprocess_state;
    SpeexEchoState *echo_state;
};

#endif // SPEEXHANDLER_H
