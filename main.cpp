#include <iostream>
#include "audio/alsahandle.h"
#include "speexhandle/speexhandler.h"
#include <speex/speex_preprocess.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include "linklist.h"
#include "opus/opus.h"
#include "common.h"
#include "gpiocontrol.h"
#include "mfcc/mfcchandle.h"
#include "keyword/keyworddetect.h"
#include "keyword/pocketsphinxkeyword.h"
#include "network/netclient.h"
#include <time.h>
#include "kwdethandler.h"

using namespace std;
using namespace Poco;


int main (int argc, char *argv[])
{
    SpeexHandler speexobj(FRAMESIZE,SAMPLERATE);
    Linklist list( AFRAMEBUFSIZE , 30 );
    KWDetHandler kwd_hd;
    speexobj.addlist(list);
    kwd_hd.addlist(list);
    Poco::Thread th;

    speexobj.start(speexobj);
    th.start(kwd_hd);

    speexobj.join();
    th.join();

    return 0;
}
