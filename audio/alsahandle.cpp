#include "alsahandle.h"

AlsaHandle::AlsaHandle()
{
    pcm_param.streamRDWR = SND_PCM_STREAM_CAPTURE ;
    pcm_param.streamaccess = SND_PCM_ACCESS_RW_INTERLEAVED;
    pcm_param.mode = 0;
    pcm_param.playback_handle = NULL;
    pcm_param.frames = defaultframe; // frame buffer size;
    pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;

    params.RATE = 44100;
    params.CHANNELS = 2;
    params.BIT = 16;
    params.DIR = 0;
    params.bytesperframe = 4;
}

AlsaHandle::AlsaHandle(struct audioparam &params)
{

}

AlsaHandle::~AlsaHandle()
{

}

void AlsaHandle::stop()
{

    if(pcm_param.playback_handle!=NULL) {
        snd_pcm_close(pcm_param.playback_handle);
        pcm_param.playback_handle=NULL;
    }
}
r_status AlsaHandle::setHW(std::string hw)
{
    this->hw=hw;

}
r_status AlsaHandle::init (int sample_rate,int channels , int sample_bit, snd_pcm_stream_t streamRDWR)
{
    pcm_param.streamRDWR = streamRDWR ;
    if(sample_bit != params.BIT)
    {
        params.BIT = sample_bit;
        switch(params.BIT) {
        case 8:pcm_param.pcm_format = SND_PCM_FORMAT_S8;break;
        case 16:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        case 24:pcm_param.pcm_format = SND_PCM_FORMAT_S24_LE;break;
        default:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        }
    }
    if ( sample_rate != params.RATE ) params.RATE = sample_rate;
    if ( channels != params.CHANNELS ) params.CHANNELS = channels;
    int bytesperfame = channels * sample_bit / 8;
    if( params.bytesperframe != bytesperfame ) params.bytesperframe = bytesperfame;
    return audioinit();
}

r_status AlsaHandle::init (int sample_rate,int channels , int sample_bit)
{
    if(sample_bit != params.BIT)
    {
        params.BIT = sample_bit;
        switch(params.BIT) {
        case 8:pcm_param.pcm_format = SND_PCM_FORMAT_S8;break;
        case 16:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        case 24:pcm_param.pcm_format = SND_PCM_FORMAT_S24_LE;break;
        default:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        }
    }
    if ( sample_rate != params.RATE ) params.RATE = sample_rate;
    if ( channels != params.CHANNELS ) params.CHANNELS = channels;
    int bytesperfame = channels * sample_bit / 8;
    if( params.bytesperframe != bytesperfame ) params.bytesperframe = bytesperfame;
    return audioinit();
}
r_status  AlsaHandle::init (struct audioparam &params)
{
    if(params.BIT != this->params.BIT)
    {
        this->params.BIT = params.BIT;
        switch(this->params.BIT) {
        case 8:pcm_param.pcm_format = SND_PCM_FORMAT_S8;break;
        case 16:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        case 24:pcm_param.pcm_format = SND_PCM_FORMAT_S24_LE;break;
        default:pcm_param.pcm_format = SND_PCM_FORMAT_S16_LE;break;
        }
    }
    if ( this->params.BIT != params.RATE ) this->params.RATE = params.RATE;
    if ( this->params.CHANNELS != params.CHANNELS ) this->params.CHANNELS = params.CHANNELS ;
    int bytesperfame = params.CHANNELS * params.BIT / 8;
    if( this->params.bytesperframe != bytesperfame ) this->params.bytesperframe = bytesperfame;
    return audioinit();

}
r_status AlsaHandle::audioinit ()
{

    //printf("in %s\n",__func__);
    int err;
    if(pcm_param.playback_handle!=NULL) {

        snd_pcm_close(pcm_param.playback_handle);
        pcm_param.playback_handle=NULL;
    }
    if(hw.size()<=0) {
    hw="default";
    }
    if (( err = snd_pcm_open (&pcm_param.playback_handle, \
                              hw.c_str(), pcm_param.streamRDWR,\
                              pcm_param.mode) ) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n", hw.c_str(), snd_strerror(err));
        pcm_param.playback_handle = NULL;
        return IFAILED;
    }
    //分配 snd_pcm_hw_params_t 结构体
    if ((err = snd_pcm_hw_params_malloc(&(pcm_param.hw_params))) < 0)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
    //初始化 hw_params
    if ((err = snd_pcm_hw_params_any(pcm_param.playback_handle, \
                                     pcm_param.hw_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }
    //初始化访问权限
    if ((err = snd_pcm_hw_params_set_access(pcm_param.playback_handle,
                                            pcm_param.hw_params,
                                            pcm_param.streamaccess)) < 0)
    {
        fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //初始化采样格式
    if ((err = snd_pcm_hw_params_set_format(pcm_param.playback_handle,
                                            pcm_param.hw_params,
                                            pcm_param.pcm_format)) < 0)
    {
        fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
    if ((err = snd_pcm_hw_params_set_rate_near(pcm_param.playback_handle,
                                               pcm_param.hw_params,
                                               (unsigned int *)(&params.RATE),
                                               (int *)(&params.DIR) )) < 0)
    {
        fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
        return IFAILED;
    }


    //设置通道数量
    if ((err = snd_pcm_hw_params_set_channels(pcm_param.playback_handle,
                                              pcm_param.hw_params,
                                              params.CHANNELS)) < 0)
    {
        fprintf(stderr, "cannot set channel count %d  (%s)\n", params.CHANNELS,snd_strerror(err));
        return IFAILED;
    }

    snd_pcm_uframes_t frames;
#ifndef AUTOSETFRAME
    frames = pcm_param.frames *4;
    if ((err = snd_pcm_hw_params_set_buffer_size_near(pcm_param.playback_handle,
                                                      pcm_param.hw_params, &frames)) <0 ){
        fprintf(stderr, "cannot set buffer size  (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    frames = pcm_param.frames;
    err = snd_pcm_hw_params_set_period_size_near(pcm_param.playback_handle,
                                                 pcm_param.hw_params, &frames,
                                                 (int *)(&params.DIR));
    if (err < 0)
    {
        printf("Unable to set period size %d : %s\n", frames,  snd_strerror(err));
    }
#endif
    //设置 hw_params
    if ((err = snd_pcm_hw_params(pcm_param.playback_handle,
                                 pcm_param.hw_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        return IFAILED;
    }
    //释放分配的 snd_pcm_hw_params_t 结构体
    snd_pcm_hw_params_free(pcm_param.hw_params);



    if((err = snd_pcm_hw_params_get_period_size ( pcm_param.hw_params,
                                                  &(frames),
                                                  &params.DIR ))< 0)
    {
        fprintf(stderr, "cannot get period size (%s)\n", snd_strerror(err));
        return IFAILED;

    }
    pcm_param.frames= frames;


    //完成硬件参数设置，使设备准备好
    if ((err = snd_pcm_prepare(pcm_param.playback_handle)) < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        return IFAILED;
    }


    if(pcm_param.frames> 0) {

        params.bufsize = pcm_param.frames * params.bytesperframe;
        DEBUG("frames size %d\n",pcm_param.frames);
    }


    return ISUCCESS;
}
r_status AlsaHandle::writei(char *buf ,int frames)
{
    int err;
    err = snd_pcm_writei ( pcm_param.playback_handle , buf, frames);
    if ( err == -EPIPE) {

        fprintf(stderr, "underrun accourred, frames:%d\n",frames);
        snd_pcm_prepare( pcm_param.playback_handle );

    } else if ( err < 0 ) {

        fprintf(stderr, "write to audio interface failed (%s) value:%d\n", snd_strerror(err),err);
        audioinit();

    } else if ( err < frames ) {

        fprintf(stderr, "short write,write %d frames\n", err);

    }
    return err;
}

r_status AlsaHandle::readi(char *buf ,int frames)
{
    int err;
    err = snd_pcm_readi ( pcm_param.playback_handle , buf, frames);
    if ( err == -EPIPE) {

        fprintf(stderr, "underrun accourred, frames:%d\n",frames);
        snd_pcm_prepare( pcm_param.playback_handle );

    } else if ( err < 0 ) {

        fprintf(stderr, "write to audio interface failed (%s) value:%d\n", snd_strerror(err),err);
        audioinit();

    } else if ( err < frames ) {

        fprintf(stderr, "short read,write %d frames\n", err);

    }
    return err;
}
