#ifndef __COMMON_H
#define __COMMON_H

/*audio configration*/
#define CHANNLE 2
#define BITS 16
#define SAMPLERATE 48000
#define FRAMESIZE (SAMPLERATE/50)
#define AFRAMEBUFSIZE (FRAMESIZE * CHANNLE *BITS /8)

#endif
