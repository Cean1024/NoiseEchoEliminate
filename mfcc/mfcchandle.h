#ifndef MFCCHANDLE_H
#define MFCCHANDLE_H
#include "common.h"
#include<complex>
#include <cstring>
#include <iostream>

using namespace std;



class RangBuffer {
public:
    RangBuffer()
    {
        buffernum = ceil(Win_Time/STEP_TIME);
        buffersize = ceil(SAMPLERATE * STEP_TIME);
        printf("in %s bufnum:%d bufsize:%d\n",__func__,buffernum,buffersize);
        framebuffer =(float **)new float[buffernum];

        for( int i=0 ; i < buffernum ; i++) {
            framebuffer[i] = new float[buffersize];
            memset(framebuffer[i],0,buffersize * sizeof(float));
        }

    }
    virtual ~RangBuffer()
    {
        for( int i = 0; i < buffernum ; i++)
            delete[] framebuffer[i];
        delete[] framebuffer;
    }
    int frame_update(float *frames)
    {
        float * tmp0,*tmp1;

        for( int i=0 ;i < 2 ; i++ ) {
            memcpy( framebuffer[i] , frames + i * buffersize ,\
                   buffersize * sizeof(float));
        }

        tmp0=framebuffer[0];
        tmp1=framebuffer[1];
        framebuffer[0]=framebuffer[2];
        framebuffer[1]=framebuffer[3];
        framebuffer[2]=framebuffer[4];
        framebuffer[3]=tmp0;
        framebuffer[4]=tmp1;


        return get_frame(frames);

    }
    int get_frame(float *frame)
    {
        for(int i=0; i<buffernum;i++)
            memcpy(frame+ i*buffersize,framebuffer[i],\
                   buffersize*sizeof(float));
        return 0;
    }



protected:

private:
    float **framebuffer;
    int buffersize;
    int buffernum;


};

class MfccHandle:public RangBuffer {
public:
    MfccHandle();

    virtual ~MfccHandle();


    /************************************
     *功能:
     * 数据状态值提取
     *
     *参数说明：
     *input:
     *  sample: 为样本
     *  len: 为样本的长度
     *  chan: num of channl
     *output:
     *  c: dct 变换后的梅尔倒谱系数
     *return :
     *  无
     ************************************
     */
    void mfcc_calc(short *sample, int len, int chan, float c[13]);

protected:

    /************************************
     *功能:
     * 高通滤波
     *
     *参数说明：
     *input:
     *  sample: 为样本
     *  len: 为样本的长度
     *  factor: 预加重系数
     *return :
     *  返回滤波的结果
     ************************************
     */
    float* pre_emphasizing(float *sample, int len, float factor);

    /************************************
     *功能:
     * 高通滤波
     *
     *参数说明：
     *input:
     *  sample: 为样本
     *  len: 为样本的长度
     *  factor: 预加重系数
     *  chan: num of channl
     *output:
     *  out:返回滤波的结果
     *return :
     *  返回状态
     ************************************
     */
    int pre_emphasizing(short *sample, float *out, int len, int chan, float factor);

    /*
     * 初始化汉明窗
     *
     */
    void Hamming( float *hamWin, int hamWinSize );

    /***********************************
     *功能:
     * 计算每一帧的功率谱
     *
     *参数说明：
     *input:
     * framSample: 加汉明窗后的音频
     * framSize: size
     *output:
     * FFTSample: 快速fft 变换后的功率谱
     ************************************
     */
    void mfccFFT(float *frameSample, float *FFTSample, int frameSize);


    /************************************
     *功能:
     * 返回计算出的fft变换结果
     *
     *参数说明：
     *input:
     *  Sample: 为对样本进行预加重之后的数组
     *  alignframeSize: 为每帧的样本点个数
     *return :
     *  FFTSample:返回计算的结果
     ************************************
     */
    int  mfccFrame(float *FFTSample, float *Sample, int alignframeSize);

    /************************************
     *功能:
     * 梅尔倒谱系数计算
     *
     *参数说明：
     *input:
     * sampleRate: 采样率
     * FFTSample: 计算出的fft变换结果
     * frameSize: size
     *
     *output:
     * mel: 梅尔倒谱系数
     ************************************
     */
    void computeMel(float mel[FILTERNUM], int sampleRate, float *FFTSample, int frameSize);


    /************************************
     *功能:
     * 离散余弦变换
     *
     *参数说明：
     *input:
     * mel: 梅尔倒谱系数
     *
     *output:
     * c: 离散余弦变换值
     ************************************
     */
    void DCT(float *mel, float *c);

private:
    float *Sample ;
    float  *FFTSample;
    float *mel;
    float factor;//预加重参数
    int hamWinSize;
    int alignframeSize;

    float *hamWin;

    float *m;
    float *h;
    float *f;


};

#endif // MFCCHANDLE_H
