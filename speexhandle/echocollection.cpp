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

    char udpdata[5120];
    spx_int16_t origin[ECHOFRAMESIZE *2];
    int oldsize = ECHOFRAMESIZE *4;
    int index=0;
    int size=oldsize;
    char *ori_p = (char *)origin;

    listnode4 datanode;
    datanode.realsize = FRAMESIZE * 4;
    while(true) {
        ret = recvData(udpdata,5120);
        //LOGOUT("EchoCollection:: recv udp data len:%d",ret);
        while (ret -index >= size) {

            memcpy(ori_p,udpdata+index,size);
            index += size;

            resample(origin,ECHOFRAMESIZE,(spx_int16_t *)datanode.data,FRAMESIZE);
            echolist->push_back(datanode);
            //LOGOUT("EchoCollection:: in size:%d",size);
            ori_p = (char *) origin;
            size = oldsize;
        }
        size = ret-index;
        memcpy(ori_p,udpdata+index,size);
        //LOGOUT("EchoCollection:: out size:%d",size);
        ori_p += size;
        size = oldsize - size;
        index =0;

    }

}
