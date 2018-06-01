#ifndef SPEEXHANDLER_H
#define SPEEXHANDLER_H
#include <iostream>

#include "common.h"
#include "speexbase.h"
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"
#include "audio/alsahandle.h"
#include <list>


class SpeexHandler:public Poco::Thread, public Poco::Runnable,public SpeexBase
{
public:
    SpeexHandler(int frames, int samplerate);
    ~SpeexHandler();

    void addlist(list<listnode2> &dlist, list<listnode2> &echo );

protected:

    void run();
private:

    AlsaHandle *reader;
    AlsaHandle *player;
    list<listnode2> *dlist;
    list<listnode2> *echolist;
};

#endif // SPEEXHANDLER_H
