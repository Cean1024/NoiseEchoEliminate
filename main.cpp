#include <iostream>
#include "audio/alsahandle.h"
//#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"

using namespace std;
using namespace Poco;

#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 32000

#define FRAMESIZE (SAMPLERATE/50)
#define AFRAMEBUFSIZE (FRAMESIZE * CHANNLE *BITS /8)
#define DENOISE_DB (20)

struct threaddata {
    AlsaHandle *read;
    AlsaHandle *write;
    SpeexPreprocessState *preprocess_state;
    Linklist *list;
};

class AudioHandle: public Poco::Runnable
{
public:
    AudioHandle() {
        data=nullptr;
    }
    void setthreaddata(threaddata *data)
    {
        this->data = data;
    }
    virtual void run()
    {
        cout <<"thread start\n";
        //spx_int16_t  buf[FRAMESIZE * CHANNLE];
        if(data == nullptr ) {
            cout <<"empty pointer!!!\n";
            return;
        }
        listnode *node;
        while(1) {
            while( (node=data->list->CreateNode()) == nullptr ) usleep(10000);
            data->read->readi(node->data,FRAMESIZE);
            //speex_echo_cancellation(echo_state,buf,buf,buf2);
            //peex_preprocess_run(data->preprocess_state ,(spx_int16_t *)node->data);
            //data.write->writei((char *)buf,FRAMESIZE);
            data->list->InsertNode(node);

        }

    }
private:
    threaddata *data;

};


int main(int argc, char *argv[])
{
    AlsaHandle audioread,audiowrite;
    threaddata data;
    string hw;

    hw="hw:1";
    audioread.setHW(hw);
    audioread.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);
    hw="hw:0";
    audiowrite.setHW(hw);
    audiowrite.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
    printf("audio init finish\n");
    //spx_int16_t  buf2[FRAMESIZE * 2];

    SpeexPreprocessState *preprocess_state = \
            speex_preprocess_state_init(FRAMESIZE*2,SAMPLERATE);

    data.preprocess_state = preprocess_state;
    data.read = &audioread;
    data.write = &audiowrite;
    Linklist list( AFRAMEBUFSIZE , 60 );
    data.list = &list;

    AudioHandle audiohandle;
    audiohandle.setthreaddata(&data);
    Poco::Thread thread;



    //SpeexEchoState *echo_state = speex_echo_state_init(defaultframe,defaultframe);
    //char resoult[FRAMESIZE];
    int ret,value;

    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_VAD,(void *)&value);

    value =95;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_PROB_START,(void *)&value);
    value =80;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_PROB_CONTINUE,(void *)&value);

    //open voice limit
    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_DENOISE,(void *)&value);
    value=DENOISE_DB;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_NOISE_SUPPRESS,(void *)&value);

    //open AGC
    value=1;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_AGC,(void *)&value);
    value=SAMPLERATE;
    speex_preprocess_ctl(preprocess_state,SPEEX_PREPROCESS_SET_AGC_LEVEL,(void *)&value);


    printf("launch thread\n");
    thread.start(audiohandle);


    listnode *node;
    char buf[AFRAMEBUFSIZE];
    char buf2[AFRAMEBUFSIZE]={'0'};


    while(1) {

        while( (node=list.GetNode()) == nullptr ) usleep(10000);

        memcpy(buf,node->data,AFRAMEBUFSIZE);
        ret = speex_preprocess_run(data.preprocess_state ,(spx_int16_t *)buf);

        if(ret) {
            audiowrite.writei(buf,FRAMESIZE);
        }
        else
            audiowrite.writei(buf2,FRAMESIZE);
        list.DestroyNode(node);
    }


    speex_preprocess_state_destroy(preprocess_state);
    //speex_echo_state_destroy（echo_state）;
    thread.join();
    return 0;
}
