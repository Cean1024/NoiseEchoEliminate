#ifndef __COMMON_H
#define __COMMON_H

/*audio configration*/
#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 16000
#define FRAMESIZE (SAMPLERATE/100)
#define AFRAMEBUFSIZE (FRAMESIZE * CHANNLE *BITS /8)





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

#endif
