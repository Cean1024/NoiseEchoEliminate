#ifndef ECHOCOLLECTION_H
#define ECHOCOLLECTION_H

#include "common.h"
#include "network/netbase.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "speexresample.h"
#include "linklist.h"
#include <list>
class EchoCollection:public NetUdpBase,public Poco::Thread,
                     public Poco::Runnable,public SpeexResample
{
public:
    EchoCollection();

    void addList(list<listnode4> &echo_list)
    {
        this->echolist = &echo_list;
    }

protected:
    void run();
    list<listnode4> *echolist;

private:
};

#endif // ECHOCOLLECTION_H
