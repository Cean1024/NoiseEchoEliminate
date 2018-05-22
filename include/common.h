#ifndef __COMMON_H
#define __COMMON_H
#include <iostream>
typedef int r_status;


/*audio configration*/
#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 16000
//#define SAMPLERATE 48000
#define FRAMESIZE (SAMPLERATE/100)
#define AFRAMEBUFSIZE (FRAMESIZE * CHANNLE *BITS /8)

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

#define CAPWAITTIME 1

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

#define LogOut(PRINTFORMET,args...) printf( "[%s] " PRINTFORMET "\n",__func__,##args)

#endif
