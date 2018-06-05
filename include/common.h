#ifndef __COMMON_H
#define __COMMON_H
#include <iostream>
#include <unistd.h>
typedef int r_status;
typedef int S_ret;

#define SoftwareName "voiceWakeUp"


/*net config*/
#define ECHO_RES_IP "127.0.0.1"
#define ECHO_RES_PORT 9998

/*audio configration*/
#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 16000
#define FRAMESIZE (SAMPLERATE/100)
#define AFRAMEBUFSIZE (FRAMESIZE * CHANNLE *BITS /8)

#define ECHOSAMPLERATE 44100
#define ECHOFRAMESIZE (ECHOSAMPLERATE/100)

#define ALSA_READ_HW "hw:1"
#define ALSA_PLAY_HW "hw:0"


/*mfcc config*/
#define FILTERNUM 40
#define DTCNUM 13
#define Win_Time 0.025//把25ms里的所有点作为一个点分析
#define Hop_Time 0.01//每隔10ms分一次帧
#define Pi 3.1415927
#define STEP_TIME 0.005 //



/*keyword config*/
//#define PATH_KEYWORD_COE "/home/pi/speech/kean/NoiseEchoEliminate/keyword/MEL_COE"
#define PATH_KEYWORD_COE "/home/pi/speech/kean/NoiseEchoEliminate/keyword/dct_meils"
enum DetectEvent {
    HaveKeyWord,
    HaveError
};

struct KeyWordInData {
    float data[DTCNUM];
};

struct KeyWordOutData {
    DetectEvent event;
    std::string key_word;
};


typedef r_status (*KeywordInCb) (float *reil_meil,void *data,int size);
typedef r_status (*DataInputCb) (void *datain , short *dataout,int size);
typedef r_status (*KeywordOutCb) (KeyWordOutData &event,void *data);


#define MODULEDIR "/home/pi/speech/dicts/xiaosang/"
#define HMM "hmm_module"
#define DICT "xiaosang.dic"
#define LM "xiaosang.lm"
#define NUMKEIWORDS 4
#define ECHOOFKEY "/home/pi/speech/kean/NoiseEchoEliminate/infoaudio/echoOfKeyword.wav"
#define CANCALOFKEY "/home/pi/speech/kean/NoiseEchoEliminate/infoaudio/cancal.wav"
//#define ECHOOFKEY "/home/pi/speech/kean/NoiseEchoEliminate/infoaudio/wozaine_ch2.wav"

#define CAPWAITTIME 2

/*common config*/


enum returnstatus {

    ERROR = -2,
    FAILED = -1,
    SUCCESS=0,
    PSUCCESS,
    PFAILED,
    FSUCCES,
    FFAILD,
    ISUCCESS,
    IFAILED,
    IGENOR,
    NEEDUPDATA,
    NOTNEEDUPDATA,
    EXIST,
    NOTEXIST,
    WAITING,
    YES,
    NO,
    FOUND,
    NOTFOUND

};

#define LOGOUT(format,...)  { \
    time_t tt=time(0); struct tm   time;\
    localtime_r(&tt,&time);\
    printf("%02d-%02d-%02d %02d:%02d:%02d" \
            " %s [%s]=> "\
            format "\n",time.tm_year + 1900,\
    time.tm_mon + 1,time.tm_mday,time.tm_hour,\
    time.tm_min,time.tm_sec,SoftwareName,\
    __func__,##__VA_ARGS__);\
  }
//printf( "[%s] " PRINTFORMET "\n",__func__,##args)

#endif
