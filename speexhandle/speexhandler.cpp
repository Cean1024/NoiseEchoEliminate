#include "speexhandler.h"

SpeexHandler::SpeexHandler(int frames,int samplerate)
{
    this->preprocess_state = speex_preprocess_state_init(frames * 2 ,samplerate);
    this->echo_state = speex_echo_state_init(frames,frames);

    int value;

    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_VAD,(void *)&value);

    value =98;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_PROB_START,(void *)&value);
    value =80;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_PROB_CONTINUE,(void *)&value);

    //open voice limit
    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_DENOISE,(void *)&value);
    value= DENOISE_DB;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,(void *)&value);

    //open AGC
    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_AGC,(void *)&value);
    value=samplerate;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_AGC_LEVEL,(void *)&value);
}
SpeexHandler::~SpeexHandler()
{
    speex_preprocess_state_destroy(preprocess_state);
    speex_echo_state_destroy(echo_state);
}
