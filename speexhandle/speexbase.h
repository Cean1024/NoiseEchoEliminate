#ifndef SPEEXBASE_H
#define SPEEXBASE_H
#include "common.h"
#include <cstring>
#include <speex/speex_preprocess.h>
#include <speex/speex_echo.h>
#include <speex/speex_resampler.h>
#include <speex/speex_jitter.h>

#define DENOISE_DB (20)

class SpeexBase
{
public:
    SpeexBase(int frames, int samplerate);
    virtual ~SpeexBase();
    S_ret echo_play(char *buf);
    S_ret audioProcess(char *data);
    void echo_reset();

private:
    SpeexPreprocessState  *preprocess_state;
    SpeexEchoState *echo_state;

    spx_int16_t *echo;
    spx_int16_t *noecho;
    int isSpeesh;

};

#endif // SPEEXBASE_H
