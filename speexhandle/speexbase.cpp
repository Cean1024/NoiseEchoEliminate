#include "speexbase.h"

SpeexBase::SpeexBase(int frames,int samplerate)
{
    this->preprocess_state = speex_preprocess_state_init(frames * 2 ,samplerate);
    this->echo_state = speex_echo_state_init(frames*2,AFRAMEBUFSIZE);


    int value;

    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_VAD,(void *)&value);

    value =98;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_PROB_START,(void *)&value);
    value =98;
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

    echo = new spx_int16_t[AFRAMEBUFSIZE/2];
    noecho = new spx_int16_t[AFRAMEBUFSIZE/2];

}
SpeexBase::~SpeexBase() {
    speex_preprocess_state_destroy(preprocess_state);
    speex_echo_state_destroy(echo_state);
    delete echo;
}
S_ret SpeexBase::echo_play(char *buf)
{
    if(echo_state == nullptr)return FAILED;

    memcpy(echo,buf,AFRAMEBUFSIZE);

    return SUCCESS;
}
void SpeexBase::echo_reset()
{
    speex_echo_state_reset(echo_state);
}
S_ret SpeexBase::audioProcess(char *data)
{
    if(echo_state == nullptr || preprocess_state == nullptr)return FAILED;
#ifdef ECHOCOLLECTIONENABLE
    speex_echo_cancellation(echo_state,(spx_int16_t *)data,echo,noecho);
#endif;
    isSpeesh = speex_preprocess_run(preprocess_state,noecho);
    memcpy(data,noecho,AFRAMEBUFSIZE);

    return isSpeesh;
}
