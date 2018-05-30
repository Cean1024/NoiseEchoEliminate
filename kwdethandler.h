#ifndef KWDETHANDLER_H
#define KWDETHANDLER_H

#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"
#include "audio/alsahandle.h"
#include "common.h"
#include "keyword/pocketsphinxkeyword.h"
#include "network/netclient.h"
#include <unistd.h>

class KWDetHandler:public Poco::Runnable
{
public:
    KWDetHandler();
    ~KWDetHandler();

    void addlist(Linklist &list);

    Linklist *getlist()
    {
        return list;
    }
    AlsaHandle *getplayer() {
        return player;
    }

protected:
    void run();
    static r_status outputCB(KeyWordOutData &event,void *data);
    static r_status DataInput(void *datain ,short *out,int size);


private:
    PocketSphinxKeyword  *PS_Kw;
    Linklist *list;
    AlsaHandle *player;



};

#endif // KWDETHANDLER_H
