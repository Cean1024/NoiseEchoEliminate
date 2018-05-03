#include "mfcchandle.h"

MfccHandle::MfccHandle()
{
    this->hamWinSize = ceil(SAMPLERATE * Win_Time);

    //计算出每帧有多少个点，然后算出最接近点的个数的2^k，使得每帧的点的个数为2^k，以便进行补0
    this->alignframeSize = (int)pow(2, ceil( log(Win_Time * SAMPLERATE) / log(2.0) ));

    Sample =new float[hamWinSize];
    FFTSample = new float[alignframeSize];
    //memset(FFTSample, 0 , alignframeSize * sizeof (float));
    mel = new float[FILTERNUM];

    hamWin = new float[hamWinSize];
    Hamming(hamWin, hamWinSize);//计算hamming窗

    factor = 0.95;//预加重参数

    m = new float[FILTERNUM + 2];
    h = new float[FILTERNUM + 2];
    f = new float[FILTERNUM + 2];


}
MfccHandle::~MfccHandle()
{
    delete[] FFTSample;
    delete[] Sample;
    delete[] mel;
    delete[] hamWin;
    delete[] m;
    delete[] h;
    delete[] f;
}
/*
    len: num of sample in 10ms
 */
void MfccHandle::mfcc_calc(short *sample, int len, int chan, float c[DTCNUM])
{
    //1.预加重
    pre_emphasizing(sample, Sample,len, chan,factor);

    frame_update(Sample);

    //printf("alignframeSize:%d len:%d\n",alignframeSize,len);

    //分帧、加窗、功率谱
    mfccFrame(FFTSample, Sample, alignframeSize);

    computeMel(mel, SAMPLERATE, FFTSample, alignframeSize);

    memset(c,0,sizeof (float) * DTCNUM);
    DCT(mel, c);

}

int MfccHandle::pre_emphasizing(short *sample,float *out,\
                                int len,int chan, float factor)
{
    out[0] = sample[ chan-1 ]*1000.0;
    int index=1;
    float tmp;
    for(int i = 1 + (chan-1)*2; i < len ; i+=chan)
    {
        //预加重过程

        out[index++] = (sample[i] - factor * sample[i - chan])*1000.0;
        //printf(" %d ",sample[i]);

    }
    //cout<<endl;
    return index;
}

//1.预加重
float* MfccHandle::pre_emphasizing(float *sample, int len, float factor)
{
    float *Sample = new float[len];
    Sample[0] = sample[0];
    for(int i = 1; i < len; i++)
    {
        //预加重过程
        Sample[i] = sample[i] - factor * sample[i - 1];
    }
    return Sample;
}

void MfccHandle::Hamming( float *hamWin, int hamWinSize )
{
    for (int i = 0; i < hamWinSize; i++)
    {
        hamWin[i] = (float)(0.54 - 0.46 * \
                             cos(2 * Pi * (float)i / ((float)hamWinSize - 1) ));
    }
}


void MfccHandle::mfccFFT(float *frameSample, float *FFTSample, int frameSize)
{
    //对分帧加窗后的各帧信号进行FFT变换得到各帧的频谱
    //并对语音信号的频谱取模平方得到语音信号的功率谱
    float dataR[frameSize];
    float dataI[frameSize];
    for(int i = 0; i < frameSize; i++)
    {
        dataR[i] = frameSample[i];
        dataI[i] = 0.0f;
        //printf(" %f ",dataR[i]);
    }
    //cout<<endl;

    int x0, x1, x2, x3, x4, x5, x6, xx, x7, x8;
    int i, j, k, b, p, L;
    float TR, TI, temp;
    /********** following code invert sequence ************/
    for(i = 0; i < frameSize; i++)
    {
        x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = 0;
        x0 = i & 0x01; x1 = (i / 2) & 0x01;
        x2 = (i / 4) & 0x01;x3 = (i / 8) & 0x01;
        x4 = (i / 16) & 0x01; x5 = (i / 32) & 0x01;
        x6 = (i / 64) & 0x01;x7 = (i / 128) & 0x01;
        x8 = (i / 256) & 0x01; xx = x0 * 256 + x1 * 128 + \
                x2 * 64 + x3 * 32 + x4 * 16 + x5 * 8 + x6 * 4 + x7 * 2 + x8;
        dataI[xx] = dataR[i];
    }
    for(i = 0; i < frameSize; i++)
    {
        dataR[i] = dataI[i]; dataI[i] = 0;
    }

    /************** following code FFT *******************/
    for(L = 1; L <= 9; L++)
    { /* for(1) */
        b = 1; i = L - 1;
        while(i > 0)
        {
            b = b * 2; i--;
        } /* b= 2^(L-1) */
        for(j = 0; j <= b-1; j++) /* for (2) */
        {
            p = 1; i = 9 - L;
            while(i > 0) /* p=pow(2,7-L)*j; */
            {
                p = p * 2; i--;
            }
            p = p * j;
            for(k = j; k < 512; k = k + 2*b) /* for (3) */
            {
                TR = dataR[k]; TI = dataI[k]; temp = dataR[k + b];
                dataR[k] = dataR[k] + dataR[k + b] * cos(2 * Pi * p / frameSize) +\
                        dataI[k + b] * sin(2 * Pi * p / frameSize);
                dataI[k] = dataI[k] - dataR[k + b] * sin(2 * Pi * p / frameSize) +\
                        dataI[k + b] * cos(2 * Pi * p / frameSize);
                dataR[k + b] = TR - dataR[k + b] * cos(2 * Pi * p / frameSize) - \
                        dataI[k + b] * sin(2 * Pi * p / frameSize);
                dataI[k + b] = TI + temp * sin(2 * Pi * p / frameSize) -\
                        dataI[k + b] * cos(2 * Pi * p / frameSize);
            } /* END for (3) */
        } /* END for (2) */
    } /* END for (1) */

    for(i = 0; i < frameSize / 2; i++)
    {
        FFTSample[i] = (dataR[i] * dataR[i] + dataI[i] * dataI[i]);
        //printf(" %f ",FFTSample[i]);
    }
    //cout<<endl;
}


int  MfccHandle::mfccFrame(float *FFTSample, float *Sample,\
                           int alignframeSize)
{


    float frameSample[alignframeSize]={0};

        for(int j = 0; j < alignframeSize; j++ )
        {
            if(j < hamWinSize ) {
                frameSample[j] = Sample[j] * hamWin[j];
            }
            else
                frameSample[j] = 0;//补0
            //printf(" %f ",frameSample[j]);
        }
        //cout<<endl;
        mfccFFT(frameSample, FFTSample, alignframeSize);


    return 0;
}

void MfccHandle::DCT(float *mel, float *c)
{
        for(int n = 0; n < DTCNUM; n++){
           for(int i = 0; i < FILTERNUM; i++){
             c[n] += mel[i] * cos(Pi * n / (2 * FILTERNUM) *  (2 * i + 1));

           }
           //printf(" %f ",c[n]);
        }
        //printf("\n");
}

void MfccHandle::computeMel(float mel[FILTERNUM], int sampleRate,\
                            float *FFTSample, int frameSize)
{
    float freMax = sampleRate / 2;//实际最大频率
    float freMin = 0;//实际最小频率
    float melFremax = 1125 * log(1 + freMax / 700);//将实际频率转换成梅尔频率
    float melFremin = 1125 * log(1 + freMin / 700);
    float k = (melFremax - melFremin) / (FILTERNUM + 1);
    for(int i = 0; i < FILTERNUM + 2; i++)
    {
        m[i] = melFremin + k * i;
        h[i] = 700 * (exp(m[i] / 1125) - 1);//将梅尔频率转换成实际频率
        f[i] = floor((frameSize + 1) * h[i] / sampleRate);
    }

        for(int j = 0; j < FILTERNUM; j++)
            mel[j] = 0;


    //计算出每个三角滤波器的输出: 对每一帧进行处理

        for(int j = 1; j <= FILTERNUM; j++)
        {
            float temp = 0;
            for(int z = 0; z < frameSize; z++)
            {
                if(z < f[j - 1])
                    temp = 0;
                else if(z >= f[j - 1] && z <= f[j])
                    temp = (z - f[j - 1]) / (f[j] - f[j - 1]);
                else if(z >= f[j] && z <= f[j + 1])
                    temp = (f[j + 1] - z) / (f[j + 1] - f[j]);
                else if(z > f[j + 1])
                    temp = 0;
                mel[j - 1] += FFTSample[z] * temp;
            }
        }


    //取对数

        for(int j = 0; j < FILTERNUM; j++)
        {
            if(mel[j] <= 0.00000000001 || mel[j] >= 0.00000000001)
                mel[j] = log(mel[j]);
        }

}

