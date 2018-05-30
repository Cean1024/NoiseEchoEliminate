#ifndef ALSAHANDLE_H
#define ALSAHANDLE_H

#include "common.h"
#include <alsa/asoundlib.h>
#include <iostream>
#define defaultframe 1024
#define n_buf 10

#define DEBUG(format,args...) printf(format,##args)

//#define ALSAPCM "/dev/pcmC0D0p"

//#define AUTOSETFRAME


struct snd_pcm_params {
    snd_pcm_t *playback_handle; //PCM 设备句柄
    snd_pcm_hw_params_t *hw_params; //硬件信息和 PCM 流配置
    snd_pcm_sw_params_t *sw_params;
    snd_pcm_sframes_t frame_to_deliver;
    snd_pcm_stream_t streamRDWR;
    snd_pcm_access_t streamaccess;
    snd_pcm_uframes_t frames;
    snd_pcm_format_t pcm_format;
    int mode;
};
struct audioparam
{
    int RATE;       //采样频率
    int BIT;       //量化位数
    int CHANNELS;   //声道数目
    int DIR;
    int bytesperframe; //bytes per frame;
    int bufsize;
};

class AlsaHandle
{
public:
    AlsaHandle();
    AlsaHandle(struct audioparam &params);
    virtual ~AlsaHandle();

    void stop();
    r_status init (int sample_rate,int channels , int sample_bit,\
                   snd_pcm_stream_t streamRDWR);
    r_status init (int sample_rate,int channels , int sample_bit);
    r_status init (struct audioparam &params);
    r_status audioinit ();
    r_status writei(char *buf ,int frames);
    r_status readi(char *buf ,int frames);
    r_status setHW(std::__cxx11::string hw);

private:

    struct snd_pcm_params pcm_param;
    struct audioparam params;
    std::string hw;
};

#endif // ALSAHANDLE_H
