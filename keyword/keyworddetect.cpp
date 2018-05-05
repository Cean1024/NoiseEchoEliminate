#include "keyworddetect.h"


r_status keyword::readmel_coe(string filepath)
{

    ifstream fp(filepath,ios::in|ios::binary);
    char buf[READLEN]={0};
    string tmp,dctmel;
    int index_x,index_y;

    for( int num = 0;num < NUM_KEYWORD; num++ ) {
        do{
            fp.getline(buf,READLEN);
            tmp=buf;
        }
        while (tmp.find("#",0)==string::npos);

        LogOut("keyword name:%s\n",buf);
        memset(buf,0,READLEN);
        fp.getline(buf,READLEN);
        LogOut("xy:%s\n",buf);
        tmp=buf;
        index_x=stoi(tmp.substr(tmp.find("x:",0)+2,3));
        index_y=stoi(tmp.substr(tmp.find("y:",0)+2,2));
        LogOut("index_x:%d index_y:%d\n",index_x,index_y);
        if(index_x >BUFFER_X || index_y > BUFFER_Y) {
            LogOut("ERROR !!! x and y should less than %d and %d\n",BUFFER_X,BUFFER_Y);
            return ERROR;
        }
        xy[num].x=index_x;
        xy[num].y=index_y;

        int begin;
        int loop=0;
        while( loop<index_x) {
            memset(buf,0,READLEN);
            fp.getline(buf,READLEN);

            begin=0;
            dctmel=strtok(buf," ");

            dct_meil[num][loop][begin] = stof(dctmel);
            //LogOut("dcymel:%f\n",dct_meil[num][loop][begin]);
            begin++;

            while( begin < index_y) {
                dctmel=strtok(NULL," ");
                dct_meil[num][loop][begin] = stof(dctmel);
                //LogOut("dcymel:%f\n",dct_meil[num][loop][begin]);
                begin++;
            }
            loop++;
        }
    }
    fp.close();
    return SUCCESS;
}



KeyWordDetect::KeyWordDetect()
{
    this->InputFunc = nullptr;
    this->OutputFunc = nullptr;
    this->Indata = nullptr;
    this->Outdata = nullptr;
    realMeil = createMatrix();
}
KeyWordDetect::~KeyWordDetect()
{
    for(int num=0;num<NUM_KEYWORD;num++) {
        for(int i=0;i<BUFFER_X;i++)
            delete[] realMeil[num][i];
        delete[] realMeil[num];
    }
    delete[] realMeil;
}

float *** KeyWordDetect::createMatrix() {

    float ***realMeil =(float ***)new float[NUM_KEYWORD];
    for(int num=0;num<NUM_KEYWORD;num++){
        realMeil[num]=(float **)new float[BUFFER_X];
        for(int i=0;i<BUFFER_X;i++){
            realMeil[num][i]=new float[BUFFER_Y];
            memset(realMeil[num][i],0,BUFFER_Y * sizeof(float));
        }
    }
    return realMeil;
}
r_status KeyWordDetect::CallbackReg(KeywordInCb inputfun,\
                                    KeywordOutCb outputfun,\
                                    void *Inputdata ,void*Outputdata)
{
    if(inputfun == nullptr || outputfun == nullptr)
        return ERROR;
    this->InputFunc = inputfun;
    this->OutputFunc = outputfun;
    this->Indata = Inputdata;
    this->Outdata = Outputdata;

    return SUCCESS;
}
r_status KeyWordDetect::DetectKeyword()
{

    if(InputFunc == nullptr || OutputFunc ==nullptr){
        LogOut("输入输出接口设置错误!\n");
        return ERROR;
    }
    while(true) {

        Process(0);

    }
}
r_status KeyWordDetect::updateMatrix(int num)
{
    if( num < 0 || num >=NUM_KEYWORD ) return ERROR;
    float *tmp;
    int size = xy[num].x -1;
    tmp = realMeil[num][size];

    for(int i=0;i < size -1 ;i++) {

        realMeil[num][i+1] = realMeil[num][i];

    }
    realMeil[num][0] = tmp;
    r_status ret = InputFunc(realMeil[num][0],Indata,BUFFER_Y);

    if ( ret != SUCCESS ) {
        LogOut("获取梅尔倒谱系数失败!\n");

    }
    return ret;
}
int KeyWordDetect::CSM(float real_Meil[BUFFER_Y], \
                       float def_Mel[BUFFER_Y], \
                       float &Similarity)
{
    double a=0,b=0,c=0;
    for( int i=0;i<BUFFER_Y;i++) {
        a += real_Meil[i] * def_Mel[i];
        b += real_Meil[i] *real_Meil[i];
        c += def_Mel[i] * def_Mel[i];
    }
    Similarity = a / (sqrt(b) * sqrt(c));
}
r_status KeyWordDetect::Process(int num)
{
    updateMatrix(num);
    float similarity=0;
    float sum=0;
    int average;
    for(int i =0;i<xy[num].x;i++) {
        CSM(realMeil[num][i],dct_meil[num][i],similarity);
        sum += similarity*100;
    }
    sum = sum/xy[num].x;
    average = (int )(sum);
    if(average > 70)
    LogOut("average %d \n",average);
    return SUCCESS;
}
