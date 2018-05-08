#include <iostream>
#include "audio/alsahandle.h"
#include "speexhandle/speexhandler.h"
#include <speex/speex_preprocess.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"
#include "opus/opus.h"
#include "common.h"
#include "gpiocontrol.h"
#include "mfcc/mfcchandle.h"
#include "keyword/keyworddetect.h"
using namespace std;
using namespace Poco;



struct threaddata {
    void *data1;
    void *data2;
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
        AlsaHandle *reader=(AlsaHandle *)data->data1;

        //spx_int16_t  buf[FRAMESIZE * CHANNLE];
        if(data == nullptr ) {
            cout <<"empty pointer!!!\n";
            return;
        }
        listnode *node;
        while(1) {
            while( (node=data->list->CreateNode()) == nullptr ) usleep(10000);
            reader->readi(node->data,FRAMESIZE);
            data->list->InsertNode(node);
        }

    }
private:
    threaddata *data;

};

void thread2_body(void *data)
{
    threaddata *body =(threaddata *)data;
    listnode *node;
    AlsaHandle *writer=(AlsaHandle *)body->data1;
    char buf[AFRAMEBUFSIZE]={'0'};
    char buf2[AFRAMEBUFSIZE];
    int err;
    OpusDecoder *  dec = opus_decoder_create(SAMPLERATE,CHANNLE,&err);
    while(1){
        while( (node=body->list->GetNode()) == nullptr )
            writer->writei(buf,FRAMESIZE);

        err = opus_decode(dec,(unsigned char*)node->data,\
                          node->realsize,(opus_int16 *)buf2,FRAMESIZE,0);
        writer->writei(buf2,FRAMESIZE);
        body->list->DestroyNode(node);
    }
    opus_decoder_destroy(dec);
}
void button_check(void *data)
{
    GpioControl gc(0,INPUT);
    while(1){

        if(gc.getvalue(0)) cout << "button stat:-------\r"<<flush;
        else cout << "button stat:pressed\r"<<flush;

        usleep(200000);
    }

}


r_status inputCB(float *reil_meil,void *data,int size)
{
    Linklist *mellist = (Linklist *)data;
    listnode *node;
    float *dctmel;
    while( (node=mellist->GetNode()) == nullptr ) usleep(10000);

    dctmel = (float *)node->data;
    if( size > DTCNUM ) return FAILED;
    memcpy(reil_meil,dctmel + DTCNUM -size, size );


/*
    for( int i=0;i<DTCNUM; i++)
        printf(" %f",dctmel[i]);

    cout<<endl;
*/
    mellist->DestroyNode(node);
    return SUCCESS;
}
 r_status outputCB (KeyWordOutData event,void *data)
 {


 }
void melnumdisplay(void *data)
{


    unsigned  int count=1;
    KeyWordDetect kwd(PATH_KEYWORD_COE);
    kwd.CallbackReg(inputCB,outputCB,data,nullptr);
    kwd.DetectKeyword();
}

int main(int argc, char *argv[])
{
    AlsaHandle audioread,audiowrite;
    SpeexHandler speexobj(FRAMESIZE,SAMPLERATE);
    threaddata data,data2;
    string hw;

    hw="hw:1";
    audioread.setHW(hw);
    audioread.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);
    hw="hw:0";
    audiowrite.setHW(hw);
    audiowrite.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
    printf("audio init finish\n");

    Linklist list( AFRAMEBUFSIZE , 30 );
    data.list = &list;
    data.data1 = (void *)&audioread;

    AudioHandle audiohandle;
    audiohandle.setthreaddata(&data);
    Poco::Thread thread,thread2,thread3;

    Linklist list2( AFRAMEBUFSIZE , 30 );
    data2.list=&list2;
    data2.data1 = &audiowrite;


    printf("launch thread\n");
    thread.start(audiohandle);
    thread2.start(thread2_body,(void *)&data2);

    listnode *node,*node2;

    //thread3.start(button_check,NULL);

    int err;
    OpusEncoder *enc = opus_encoder_create(SAMPLERATE,CHANNLE,\
                                           OPUS_APPLICATION_AUDIO,&err);
    //GpioControl GC;
    //int ledflag=0;


    Linklist mellist( DTCNUM * sizeof(float) , 30 );
    MfccHandle  MfccCalc;
    listnode *melnode;
    thread3.start(melnumdisplay,(void *)&mellist);



    while(1) {

        while( (node=list.GetNode()) == nullptr ) usleep(10000);

        //memcpy(buf,node->data,AFRAMEBUFSIZE);

        //判断是否有语音数据
        if( speexobj.audioprocess(node->data) ) {
/*
            //有则将有语音数据的buf 通过opus 编码后 保存到另外一个list中
            if( (node2=data2.list->CreateNode()) != nullptr ) {
                err = opus_encode(enc,(opus_int16 *)node->data,\
                                  FRAMESIZE,(unsigned char *)node2->data,\
                                  AFRAMEBUFSIZE);

                node2->realsize =err;
                data2.list->InsertNode(node2);

            }
*/
            if( (melnode=mellist.CreateNode()) != nullptr ) {
                MfccCalc.mfcc_calc((opus_int16 *)node->data,\
                                   FRAMESIZE * CHANNLE,CHANNLE,(float *)melnode->data);
                mellist.InsertNode(melnode);

            }


        }

        list.DestroyNode(node);

    }

    opus_encoder_destroy(enc);
    thread.join();
    return 0;
}
