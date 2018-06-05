#include "speexhandle/speexhandler.h"
#include <Poco/Thread.h>
#include "linklist.h"
#include "common.h"
//#include "gpiocontrol.h"
#include "kwdethandler.h"
#include "speexhandle/echocollection.h"

using namespace std;
using namespace Poco;


int main (int argc, char *argv[])
{
    SpeexHandler speexobj(FRAMESIZE,SAMPLERATE);
    KWDetHandler kwd_hd;
    EchoCollection echo_hd;
    list<listnode2> dlist;
    list<listnode4> echodlist;

    speexobj.addlist(dlist,echodlist);
    kwd_hd.addlist(dlist);
    echo_hd.addList(echodlist);
    Poco::Thread th;

    speexobj.start(speexobj);
    th.start(kwd_hd);
    echo_hd.start(echo_hd);

    speexobj.join();
    th.join();

    return 0;
}
