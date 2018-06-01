#include "speexhandler.h"

SpeexHandler::SpeexHandler(int frames,int samplerate):SpeexBase(frames,samplerate)
{
    player =nullptr;
    reader = new AlsaHandle();
    reader->setHW(ALSA_READ_HW);
    int ret = reader->init(samplerate,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);
    if(ret != ISUCCESS)  LOGOUT("reader->init failed!!");
    dlist = nullptr;
/*
    player = new AlsaHandle();
    player->setHW(ALSA_PLAY_HW);
    player->init(samplerate,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
*/
}
SpeexHandler::~SpeexHandler()
{
    if(reader )delete reader;
}

void SpeexHandler::addlist(list<listnode2> &dlist, list<listnode2> &echo )
{
    this->echolist =&echo;
    this->dlist = &dlist;
}
#include <sys/time.h>
void SpeexHandler::run()
{
    char buf[AFRAMEBUFSIZE];
    char echobuf[AFRAMEBUFSIZE];
    short *p1,*p2;
    if( dlist == nullptr ) { LOGOUT("datalist is empty!!"); return;}
    memset(echobuf,0,AFRAMEBUFSIZE);

    listnode2 datanode;
#ifdef CALCRUNNINGTIME
    struct timeval tpstart,tpend;
    float timeuse;
#endif

    while(true) {

#ifdef CALCRUNNINGTIME
        gettimeofday(&tpstart,NULL);
#endif

        reader->readi(buf,FRAMESIZE);
        //echo_play(echobuf);
        datanode.speechflag = 0;
        datanode.speechflag = audioProcess(buf);

        //if(player) player->writei(node->data,FRAMESIZE);

        /*change channle 2 -> 1*/
        p1=(short *)datanode.data;
        p2=(short *)buf;
        for(int i = 0 ;i < FRAMESIZE ; i++) {
            *p1++=*p2;
            p2+=2;
        }

        datanode.realsize = FRAMESIZE * CHANNLE;
        dlist->push_back(datanode);
#ifdef CALCRUNNINGTIME
        gettimeofday(&tpend,NULL);
        timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
        tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000000;
        printf("Used Time:%f\n",timeuse);
#endif
    }
}
