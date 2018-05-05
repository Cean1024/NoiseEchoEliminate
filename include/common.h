#ifndef __COMMON_H
#define __COMMON_H

/*audio configration*/
#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 16000
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

/*common config*/
#define r_status int

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
    PLAYSTART,
    PLAYSTOP,
    PLAYRUNING,
    PLAYWAITING
};

#define LogOut(PRINTFORMET,args...) printf( "[%s] " PRINTFORMET,__func__,##args)

#endif
