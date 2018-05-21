#ifndef KEYWORDDETECT_H
#define KEYWORDDETECT_H

#include "common.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
//#include <vector>

#define READLEN 200

#define BUFFER_X 70
#define BUFFER_Y 12
#define NUM_KEYWORD 4


using namespace std;


struct x_y {
  int x;
  int y;
  char keyword[10];
  int acount;
};

class keyword
{
public:
    keyword() {
        dct_meil =(float ***)new float[NUM_KEYWORD];
        for(int num=0;num<NUM_KEYWORD;num++){
            dct_meil[num]=(float **)new float[BUFFER_X];
            for(int i=0;i<BUFFER_X;i++){
                dct_meil[num][i]=new float[BUFFER_Y];
                memset(dct_meil[num][i],0,BUFFER_Y * sizeof(float));
            }
        }
        memset(&xy,0,sizeof(x_y)*NUM_KEYWORD);
    }
    virtual ~keyword() {

        for(int num=0;num<NUM_KEYWORD;num++) {
            for(int i=0;i<BUFFER_X;i++)
                delete[] dct_meil[num][i];
            delete[] dct_meil[num];
        }
        delete[] dct_meil;

    }

    r_status readmel_coe(string filepath);

protected:
    float ***dct_meil;
    x_y xy[NUM_KEYWORD];

};

class KeyWordDetect:public keyword
{

public:
    KeyWordDetect();
    KeyWordDetect(string filepath)
    {
        dctfile=filepath;
        r_status ret;
        ret = readmel_coe(dctfile);
        if(ret == ERROR) LogOut("readmel_coe failed\n");

        realMeil = createMatrix();
    }
    virtual ~KeyWordDetect();

    r_status CallbackReg(KeywordInCb inputfun,\
                         KeywordOutCb outputfun,\
                         void *Inputdata ,void*Outputdata);

    r_status DetectKeyword();

protected:
    r_status CSM(float real_Meil[BUFFER_Y],\
                 float def_Mel[BUFFER_Y],\
                 float  &similarity);
    r_status Process(int num);
    r_status updateMatrix (int num);


private:

    float *** createMatrix();

    string dctfile;
    KeywordInCb InputFunc;
    KeywordOutCb OutputFunc;
    void *Indata;
    void *Outdata;

    float ***realMeil;
};

#endif // KEYWORDDETECT_H
