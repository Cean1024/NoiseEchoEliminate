#include <iostream>
#include "audio/alsahandle.h"
using namespace std;

int main(int argc, char *argv[])
{
    AlsaHandle audioread,audiowrite;
    string hw;
    hw="hw:1";
    audioread.setHW(hw);
    audioread.init(44100,2,16,SND_PCM_STREAM_CAPTURE);
    hw="hw:0";
    audiowrite.setHW(hw);
    audiowrite.init(44100,2,16,SND_PCM_STREAM_PLAYBACK);
    char buf[4096];
    while(1){
        audioread.readi(buf,1024);
        audiowrite.writei(buf,1024);
    }

    return 0;
}
