#include "speexhandler.h"

SpeexHandler::SpeexHandler(int frames,int samplerate):SpeexBase(frames,samplerate)
{
    player =nullptr;
#ifdef RECENABLE
    reader = new AlsaHandle();
    reader->setHW(ALSA_READ_HW);
    int ret = reader->init(samplerate,CHANNLE,BITS,SND_PCM_STREAM_CAPTURE);
    if(ret != ISUCCESS)  LOGOUT("reader->init failed!!");
#endif
    dlist = nullptr;
#ifdef PLAYECHO
    player = new AlsaHandle();
    player->setHW("plughw:1");
    player->init(samplerate,2,BITS,SND_PCM_STREAM_PLAYBACK);
#endif
}
SpeexHandler::~SpeexHandler()
{
    if(reader )delete reader;
}

void SpeexHandler::addlist(list<listnode2> &dlist, list<listnode4> &echo )
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

#ifdef ECHOCOLLECTIONENABLE
    listnode4 echonode;
    list<listnode4>::iterator it;
#endif
    int count=0;
    int delaycounts=3;
    int beforcount=0;
    while(true) {

#ifdef CALCRUNNINGTIME
        gettimeofday(&tpstart,NULL);
#endif

#ifdef RECENABLE
        reader->readi(buf,FRAMESIZE);
#endif
        if(beforcount > 0) {
            beforcount--;
            continue;
        }

        //if(player) player->writei( buf , FRAMESIZE);
#ifdef ECHOCOLLECTIONENABLE
        do {
        if(!echolist->empty()) {
            //LOGOUT("handle echo");
            it = echolist->begin();
            echonode =*it;

            if(echonode.resetflag) {
                LOGOUT("reset echo config");
                count=0;
                 //beforcount=13;

                if(echonode.ori_rate == 24000) {
                    delaycounts = 0;
                    beforcount=0;
                    //continue;
                }
                else {
                    //echo_reset();
                    beforcount=5;
                    //delaycounts =3;
                }
            }
            //if(player) player->writei( echonode.data , FRAMESIZE);
            if(beforcount<0)beforcount++;
            echo_play(echonode.data);
            echolist->erase(it);
        }
        }while(beforcount < 0);

   // } else count++;
#endif
#ifdef RECENABLE
        datanode.speechflag = audioProcess(buf);
#ifdef PLAYECHO
       // if(player) player->writei(buf,FRAMESIZE);
#endif
        /*change channle 2 -> 1*/
        p1=(short *)datanode.data;
        p2=(short *)buf;
        for(int i = 0 ;i < FRAMESIZE ; i++) {
            *p1++=*p2;
            p2+=2;
        }

        datanode.realsize = FRAMESIZE * CHANNLE;
        dlist->push_back(datanode);
#endif

#ifdef CALCRUNNINGTIME
        gettimeofday(&tpend,NULL);
        timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+
        tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000000;
        printf("Used Time:%f\n",timeuse);
#endif

    }
}
