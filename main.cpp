#include "speexhandle/speexhandler.h"
#include <Poco/Thread.h>
#include "linklist.h"
#include "common.h"
//#include "gpiocontrol.h"
#include "kwdethandler.h"

using namespace std;
using namespace Poco;


int main (int argc, char *argv[])
{
    SpeexHandler speexobj(FRAMESIZE,SAMPLERATE);
    KWDetHandler kwd_hd;
    list<listnode2> dlist;
    list<listnode2> echodlist;

    speexobj.addlist(dlist,echodlist);
    kwd_hd.addlist(dlist);
    Poco::Thread th;

    speexobj.start(speexobj);
    th.start(kwd_hd);

    speexobj.join();
    th.join();

    return 0;
}
