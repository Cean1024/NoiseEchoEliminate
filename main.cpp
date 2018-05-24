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
#include "keyword/pocketsphinxkeyword.h"
#include "network/netclient.h"

using namespace std;
using namespace Poco;



struct threaddata {
    void *data1;
    void *data2;
    Linklist *list;
    bool needCap;
    int is_speech;
    int Captime;
    time_t times;
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

    netclient nclient;

#if 1

    OpusDecoder * dec = opus_decoder_create(SAMPLERATE,CHANNLE,&err);
    while(1) {
        while( (node=body->list->GetNode()) == nullptr ) usleep(100000);
            //writer->writei(buf,FRAMESIZE);

        err = opus_decode(dec,(unsigned char*)node->data,\
                          node->realsize,(opus_int16 *)buf2,FRAMESIZE,0);
        //writer->writei(buf2,FRAMESIZE);
        nclient.senddata(buf2,AFRAMEBUFSIZE);
        body->list->DestroyNode(node);
    }
    opus_decoder_destroy(dec);

#endif

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
 r_status outputCB (KeyWordOutData &event,void *data)
 {
     threaddata *tmp=(threaddata *)data;
     AlsaHandle *writer = (AlsaHandle *) tmp->data1;
     threaddata *tmp2 = (threaddata *)tmp->data2;
    listnode *node;

    char buf[AFRAMEBUFSIZE];
    int ret,err;
    OpusDecoder * dec = opus_decoder_create(SAMPLERATE,CHANNLE,&err);


    if(event.event == HaveKeyWord) {
        netclient nclient;
        LogOut("keyword [%s] occoured!!",event.key_word.c_str());

        writer->init (SAMPLERATE,2,BITS,SND_PCM_STREAM_PLAYBACK);
        int fd=open(ECHOOFKEY,O_RDONLY);

        read(fd,buf,44);
        while ((ret=read(fd,buf,AFRAMEBUFSIZE)) > 0) {
            writer->writei(buf,FRAMESIZE);
            memset(buf,0,AFRAMEBUFSIZE);
        }
        close(fd);
        /*set cap wait time*/
        tmp->Captime=CAPWAITTIME;
        tmp->needCap=true;



        while ( (tmp->needCap == true) ) {
            if ((node = tmp2->list->GetNode()) != nullptr)  {

            err = opus_decode(dec,(unsigned char*)node->data,\
                              node->realsize,(opus_int16 *)buf,FRAMESIZE,0);
            //writer->writei(buf,FRAMESIZE);

            short *p1,*p2;

            p1 = (short *)buf;
            p2 = (short *)buf;

            for(int i=0;i< FRAMESIZE;i++) {
                *p1++ = *p2;
                p2+=2;
            }

            nclient.senddata(buf,AFRAMEBUFSIZE/2);

            //nclient.senddata(node->data,node->realsize);
            tmp2->list->DestroyNode(node);

            } else usleep(10000);
        }

        fd = open(CANCALOFKEY,O_RDONLY);
        read(fd,buf,44);
        while ((ret=read(fd,buf,AFRAMEBUFSIZE)) > 0) {
            writer->writei(buf,FRAMESIZE);
            memset(buf,0,AFRAMEBUFSIZE);
        }
        close(fd);
        writer->stop();

        nclient.s_close();

    }
 }

r_status DataInput (void *datain ,short *out,int size)
{
    Linklist *mellist = (Linklist *)datain;
    listnode *node;

    while( (node=mellist->GetNode()) == nullptr ) usleep(10000);
    memcpy((char *)out,(char *)node->data,size*2);
    mellist->DestroyNode(node);
    return SUCCESS;

}
void melnumdisplay(void *data)
{
    threaddata *tmp= (threaddata *)data;

    PocketSphinxKeyword kwd(MODULEDIR HMM,MODULEDIR LM ,MODULEDIR DICT);
    kwd.registeInOut(DataInput,tmp->list,outputCB,data);
    kwd.detectKeyword();

}


r_status readmic (void *datain ,short *out,int size)
{
    AlsaHandle *mic = (AlsaHandle *)datain;
    char *buf= new char[AFRAMEBUFSIZE];
    short *p1;
    p1=(short *)buf;
    memset(buf,0,FRAMESIZE);
    mic->readi(buf,size);
    for(int i=0;i<size;i++) {
        *out++=*p1;
        p1+=2;
    }
    delete[] buf;
    return SUCCESS;

}
int main (int argc, char *argv[])
{
    AlsaHandle audioread,audiowrite;
    SpeexHandler speexobj(FRAMESIZE,SAMPLERATE);
    threaddata data,data2,data3;
    string hw;

#if 0
    //netclient nclient("192.168.199.245",9009);
    netclient nclient;
    nclient.senddata("hallo world",12);
    nclient.s_close();
#endif
    hw="hw:1";
    audioread.setHW(hw);
    audioread.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);

#if 1
    hw="hw:0";
    audiowrite.setHW(hw);
    //audiowrite.init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
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
    //thread2.start(thread2_body,(void *)&data2);

    listnode *node,*node2;


    int err;
    OpusEncoder *enc = opus_encoder_create(SAMPLERATE,CHANNLE,\
                                           OPUS_APPLICATION_AUDIO,&err);

    Linklist list3 ( FRAMESIZE * 2 , 30 );
    listnode *keynode;
    data3.list = &list3;
    data3.data1 = (void *) &audiowrite;
    data3.data2 = &data2;
    data3.needCap = false;
    data3.times = 0;
    thread3.start(melnumdisplay,(void *)&data3);


    while(1) {

        while( (node=list.GetNode()) == nullptr ) usleep(10000);

        //memcpy(buf,node->data,AFRAMEBUFSIZE);

        //对数据进行降噪处理并判断是否有语音数据
        data3.is_speech = speexobj.audioprocess(node->data);

        if(data3.is_speech ) {
        if( (keynode=list3.CreateNode()) != nullptr ) {
            short *p1,*p2;
            p1=(short *)keynode->data;
            p2=(short *)node->data;
            for(int i = 0 ;i < FRAMESIZE ; i++) {
                *p1++=*p2;
                p2+=2;
            }
            list3.InsertNode(keynode);
        }
        }

        if( data3.needCap  ) {
           if( data3.is_speech ) {
#if 1
            //有则将有语音数据的buf 通过opus 编码后 保存到另外一个list中
            if( (node2=data2.list->CreateNode()) != nullptr ) {
                err = opus_encode(enc,(opus_int16 *)node->data,\
                                  FRAMESIZE,(unsigned char *)node2->data,\
                                  AFRAMEBUFSIZE);

                node2->realsize = err;
                data2.list->InsertNode(node2);
            }

            } else {
               time_t t=time(0);
               //LogOut("get time :%lu",t);
               if( data3.times == 0 )
                   data3.times=t;
               else if( (t - data3.times ) > data3.Captime) {
                   data3.needCap=false;
                   data3.times = 0;
               }
#endif
           }


        }
        //time_t t=time(0);
        //LogOut("get time :%lu",t);

        list.DestroyNode(node);

    }

    opus_encoder_destroy(enc);
    thread.join();
#endif
    return 0;
}
