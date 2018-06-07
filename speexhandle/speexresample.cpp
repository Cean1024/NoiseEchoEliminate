#include "speexresample.h"

SpeexResample::SpeexResample()
{
    this->res = speex_resampler_init( 2 , MAXECHOSAMPLERATE , SAMPLERATE , 10 , NULL);
}
SpeexResample::~SpeexResample()
{
    if(res) {
        speex_resampler_destroy(res);
        res=nullptr;
    }
}
int SpeexResample::setrate(spx_uint16_t in_rate)
{
    //speex_resampler_destroy(res);
    //this->res = speex_resampler_init( CHANNLE , in_rate , SAMPLERATE , 8 , NULL);
    speex_resampler_set_rate(res,in_rate,SAMPLERATE);
    speex_resampler_reset_mem(res);
}
int SpeexResample::getrate(spx_uint32_t &in, spx_uint32_t &out)
{
    speex_resampler_get_rate(res,&in,&out);
}
int  SpeexResample::resample( spx_int16_t *in,
                              spx_uint32_t in_len,
                              spx_int16_t *out,
                              spx_uint32_t out_len)
{
    return speex_resampler_process_interleaved_int(res,in,&in_len,out,&out_len);
}
