#include "kwdethandler.h"

KWDetHandler::KWDetHandler()
{
    PS_Kw = new PocketSphinxKeyword(MODULEDIR HMM,MODULEDIR LM ,MODULEDIR DICT);

    list = nullptr;
    player = new AlsaHandle;
    player->setHW(ALSA_PLAY_HW);
    //player->init(SAMPLERATE,CHANNLE,BITS,SND_PCM_STREAM_PLAYBACK);
}

KWDetHandler::~KWDetHandler()
{
    delete PS_Kw;
}

void KWDetHandler::addlist(Linklist &list)
{
    this->list = &list;
    PS_Kw->registeInOut(DataInput,this,outputCB,this);
}

r_status KWDetHandler::DataInput (void *datain ,short *out,int size)
{
    //Linklist *mellist = (Linklist *)datain;
    KWDetHandler *kwd_hd=(KWDetHandler *)datain;
    //AlsaHandle *player = kwd_hd->getplayer();
    Linklist *mellist = kwd_hd->getlist();

    listnode *node;
    if(mellist ==nullptr) return FAILED;

    while( (node=mellist->GetNode()) == nullptr ) usleep(10000);
    memcpy((char *)out,(char *)node->data,size*2); /*size *2 becuse out type is short*/
    //LOGOUT("get data isspeech:%d",node->speechflag);
    //player->writei(node->data,size);
    mellist->DestroyNode(node);
    return SUCCESS;
}

r_status KWDetHandler::outputCB (KeyWordOutData &event,void *data)
{
    KWDetHandler *kwd_hd=(KWDetHandler *)data;
    AlsaHandle *player = kwd_hd->getplayer();
    Linklist  *llist = kwd_hd->getlist();


    listnode *O_node =nullptr;

    char buf[AFRAMEBUFSIZE];
    int ret;
#ifdef OPUS_COMPRESSION
    OpusDecoder * dec = opus_decoder_create(SAMPLERATE,CHANNLE,&err);
#endif

    if(llist == nullptr) return FAILED;

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

        while ( true ) {
            O_node = llist->GetNode();
            if ( O_node != nullptr ) {;
                if(O_node->speechflag ) {
                    nclient.senddata(O_node->data,O_node->realsize);
                }  else {
                    now = time(0);
                    if((now - start) > CAPWAITTIME ) break;
                }
            llist->DestroyNode(O_node);
            //LOGOUT("GetNode");
            } else {
            //    LOGOUT("GetNode failed");
                usleep(10000);
            }
            #if 0

            if(node->speechflag )  {

#ifdef OPUS_COMPRESSION
                err = opus_decode(dec,(unsigned char*)node->data,\
                                  node->realsize,(opus_int16 *)buf,FRAMESIZE,0);
#else
                //memcpy(buf,node->data,node->realsize);
#endif
                //player->writei(buf,FRAMESIZE);

                //nclient.senddata(buf,AFRAMEBUFSIZE/2);

                nclient.senddata(node->data,node->realsize);



            }  else {
               // now = time(0);
               // if((now - start) > CAPWAITTIME ) break;
                count++;
                if(count>100000) break;
            }
            llist->DestroyNode(node);
             #endif
        }


        fd = open(CANCALOFKEY,O_RDONLY);
        read(fd,buf,44);
        while ((ret=read(fd,buf,AFRAMEBUFSIZE)) > 0) {
            player->writei(buf,FRAMESIZE);
            memset(buf,0,AFRAMEBUFSIZE);
        }
        close(fd);
        player->stop();

        nclient.s_close();
    }
}
void KWDetHandler::run()
{
    if(list==nullptr){LOGOUT("in KWDetHandler: list not set!!! thread return");return;}
     PS_Kw->detectKeyword();
}
