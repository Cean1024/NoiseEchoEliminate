#include "echocollection.h"

EchoCollection::EchoCollection()
{

}
void EchoCollection::run()
{
    S_ret ret;
    ret = open(ECHO_RES_IP,ECHO_RES_PORT);
    if(ret != SUCCESS) {LOGOUT("udp open open failed");return;}
    if(echolist == nullptr){LOGOUT("EchoCollection echolist not set!!");return;}

    spx_int16_t origin[MAXECHOFRAMESIZE *2];
    int oldsize = MAXECHOFRAMESIZE *4;
    int index=0;
    int size=oldsize;
    char *ori_p = (char *)origin;

    listnode4 datanode;
    udppackage package;
    datanode.realsize = FRAMESIZE * 4;
    int samplerate=MAXECHOSAMPLERATE;
    spx_uint32_t inrate,outrate;
    while(true) {
        ret = recvData((char *)&package,sizeof (udppackage));
        //LOGOUT("EchoCollection:: recv udp package len:%d srate:%d ch:%d psize:%d",\
               ret,package.samplerate,package.ch,package.size);
        ret = package.size;

        if(package.samplerate != samplerate) {

            setrate(package.samplerate);
            getrate(inrate,outrate);
            LOGOUT("reset samplerate old:%d new:%d inrate:%u outrate:%u"
                   ,samplerate,package.samplerate,inrate,outrate);
            samplerate = package.samplerate;
            oldsize = samplerate / 100 * 4;
            ori_p = (char *) origin;
            size = oldsize;
            index=0;
            datanode.resetflag=true;
            datanode.ori_rate=samplerate;
        }
        while (ret -index >= size) {

            memcpy(ori_p,package.buf+index,size);
            index += size;

            resample(origin,samplerate / 100,(spx_int16_t *)datanode.data,FRAMESIZE);
            echolist->push_back(datanode);
            datanode.resetflag=false;
            //LOGOUT("EchoCollection:: in size:%d",size);
            ori_p = (char *) origin;
            size = oldsize;
        }
        size = ret-index;
        if(size >0) {
        memcpy(ori_p,package.buf+index,size);
        //LOGOUT("EchoCollection:: out size:%d",size);
        ori_p += size;
        size = oldsize - size;
        } else {
            ori_p = (char *) origin;
            size = oldsize;
        }
        index =0;

    }

}
