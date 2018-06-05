#include "kwdethandler.h"

KWDetHandler::KWDetHandler()
{
    PS_Kw = new PocketSphinxKeyword(MODULEDIR HMM,MODULEDIR LM ,MODULEDIR DICT);

    dlist = nullptr;
    player = new AlsaHandle;
    if(player) player->setHW(ALSA_PLAY_HW);
    //player->init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
}

KWDetHandler::~KWDetHandler()
{
    delete PS_Kw;
}

void KWDetHandler::addlist( list<listnode2> &dlist)
{
    this->dlist = &dlist;
    PS_Kw->registeInOut(DataInput,this,outputCB,this);
}

r_status KWDetHandler::DataInput (void *datain ,short *out,int size)
{
    KWDetHandler *kwd_hd=(KWDetHandler *)datain;
    //AlsaHandle *player = kwd_hd->getplayer();
    list<listnode2> *dlist = kwd_hd->getdlist();
    //list<listnode2> *dlist = (list<listnode2> *)datain;

    listnode2 dnode;

    if(dlist == nullptr) return FAILED;

    list<listnode2>::iterator it;

    while( dlist->empty() ) {
        //LOGOUT("waiting");
        usleep(100000);
    }

    it = dlist->begin();
    dnode = *it;
    memcpy((char *)out,dnode.data,dnode.realsize); /*size *2 becuse out type is short*/

    dlist->erase(it);
    return SUCCESS;
}

r_status KWDetHandler::outputCB (KeyWordOutData &event,void *data)
{
    KWDetHandler *kwd_hd=(KWDetHandler *)data;
    AlsaHandle *player = kwd_hd->getplayer();

    list<listnode2> *dlist = kwd_hd->getdlist();
    listnode2 datanode;



    char buf[AFRAMEBUFSIZE];
    int ret;
#ifdef OPUS_COMPRESSION
    OpusDecoder * dec = opus_decoder_create(SAMPLERATE,CHANNLE,&err);
#endif


    if( event.event == HaveKeyWord ) {
        netclient nclient;
        LOGOUT("keyword [%s] occoured!!",event.key_word.c_str());

        player->init(SAMPLERATE,2,BITS,SND_PCM_STREAM_PLAYBACK);
        int fd=open(ECHOOFKEY,O_RDONLY);

        read(fd,buf,44);
        while ((ret=read(fd,buf,AFRAMEBUFSIZE)) > 0) {
            player->writei(buf,FRAMESIZE);
            memset(buf,0,AFRAMEBUFSIZE);
        }
        close(fd);

        /*set cap wait time*/
        time_t start = time(0);
        time_t now ;

        list<listnode2>::iterator it;
        bool needCap =true;
        while ( needCap ) {

            if( !dlist->empty() ) {
                it = dlist->begin();
                datanode = *it;
                dlist->erase(it);
                if(datanode.speechflag) {
                    nclient.senddata(datanode.data,datanode.realsize);
                } else {
                    now = time(0);
                    if( (now - start)  > CAPWAITTIME) needCap = false;
                }

            } else usleep(10000);

        }

        fd = open(CANCALOFKEY,O_RDONLY);
        if(fd >0) {
        read(fd,buf,44);
        while ((ret=read(fd,buf,AFRAMEBUFSIZE)) > 0) {
            player->writei(buf,FRAMESIZE);
            memset(buf,0,AFRAMEBUFSIZE);
        }
        close(fd);
        }
        player->stop();

        nclient.s_close();
        LOGOUT("KWDetHandler finish keyword handle");
    }
}
void KWDetHandler::run()
{
    if(dlist==nullptr){LOGOUT("in KWDetHandler: dlist not set!!! thread return");return;}
     PS_Kw->detectKeyword();
}
