#include "speexresample.h"

SpeexResample::SpeexResample()
{
    this->res = speex_resampler_init( 2 , 44100 , SAMPLERATE , 10 , NULL);
}
SpeexResample::~SpeexResample()
{
    if(res) {
        speex_resampler_destroy(res);
        res=nullptr;
    }
}
int  SpeexResample::resample( spx_int16_t *in,
                              spx_uint32_t in_len,
                              spx_int16_t *out,
                              spx_uint32_t out_len)
{
    return speex_resampler_process_interleaved_int(res,in,&in_len,out,&out_len);
}
