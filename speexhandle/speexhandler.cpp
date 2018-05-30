#include "speexhandler.h"

SpeexHandler::SpeexHandler(int frames,int samplerate)
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

    datalist = nullptr;
    reader = new AlsaHandle();
    reader->setHW(ALSA_READ_HW);
    int ret = reader->init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);
    if(ret != ISUCCESS)  LOGOUT("reader->init failed!!");

}
SpeexHandler::~SpeexHandler()
{
    speex_preprocess_state_destroy(preprocess_state);
    speex_echo_state_destroy(echo_state);
    if(reader )delete reader;
}

int SpeexHandler::audioprocess(char *buf)
{
    return speex_preprocess_run(preprocess_state,(spx_int16_t *)buf);
}
void SpeexHandler::addlist(Linklist &list)
{
    this->datalist = &list;
}
void SpeexHandler::run()
{
    listnode *node;
    int isSpeech;
    char buf[AFRAMEBUFSIZE];
    short *p1,*p2;
    if(datalist==nullptr) { LOGOUT("datalist is empty!!"); return;}
    while(true) {
        reader->readi(buf,FRAMESIZE);
        while ( (node=datalist->CreateNode()) == nullptr ) usleep(10000);
        speex_echo_capture(echo_state,(spx_int16_t *)buf,(spx_int16_t *)node->data);
        memcpy(node->data, buf,AFRAMEBUFSIZE);
        isSpeech = speex_preprocess_run(preprocess_state,(spx_int16_t *)node->data);
        node->speechflag = isSpeech;

        /*change channle 2 -> 1*/
        p1=(short *)node->data;
        p2=(short *)node->data;
        for(int i = 0 ;i < FRAMESIZE ; i++) {
            *p1++=*p2;
            p2+=2;
        }
        node->realsize = AFRAMEBUFSIZE/2;

        datalist->InsertNode(node);
    }
}
