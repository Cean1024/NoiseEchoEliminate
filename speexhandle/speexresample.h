#ifndef SPEEXRESAMPLE_H
#define SPEEXRESAMPLE_H
#include "common.h"
#include "speex/speex_resampler.h"

class SpeexResample
{
public:
    SpeexResample();
    virtual ~SpeexResample();

protected:
    int  resample(spx_int16_t *in,
                   spx_uint32_t in_len,
                   spx_int16_t *out,
                   spx_uint32_t out_len);

private:
    SpeexResamplerState  *res;
};

#endif // SPEEXRESAMPLE_H
