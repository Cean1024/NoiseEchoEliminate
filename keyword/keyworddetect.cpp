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
        while ( tmp.find("#",0) == string::npos );
        tmp = tmp.substr(tmp.find(".",0)-3,3);
        memcpy(xy[num].keyword,tmp.c_str(),3);
        LOGOUT("keyword name:%s\n",tmp.c_str());
        memset(buf,0,READLEN);
        fp.getline(buf,READLEN);
        LOGOUT("xy:%s\n",buf);
        tmp=buf;
        index_x=stoi(tmp.substr(tmp.find("x:",0)+2,3));
        index_y=stoi(tmp.substr(tmp.find("y:",0)+2,2));
        LOGOUT("index_x:%d index_y:%d\n",index_x,index_y);
        if(index_x >BUFFER_X || index_y > BUFFER_Y) {
            LOGOUT("ERROR !!! x and y should less than %d and %d\n",BUFFER_X,BUFFER_Y);
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
            //LOGOUT("dcymel:%f\n",dct_meil[num][loop][begin]);
            begin++;

            while( begin < index_y) {
                dctmel=strtok(NULL," ");
                dct_meil[num][loop][begin] = stof(dctmel);
                //LOGOUT("dcymel:%f\n",dct_meil[num][loop][begin]);
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
        LOGOUT("输入输出接口设置错误!\n");
        return ERROR;
    }
    while(true) {

        Process(0);
        // Process(1);
        // Process(2);
        // Process(3);

    }
}
r_status KeyWordDetect::updateMatrix(int num)
{
    if( num < 0 || num >=NUM_KEYWORD ) return ERROR;
    float *tmp;
    int size = xy[num].x -1;
    tmp = realMeil[num][0];

    for(int i=0;i < size  ;i++) {

        realMeil[num][i] = realMeil[num][i+1];

    }
    realMeil[num][size] = tmp;
    r_status ret = InputFunc(tmp,Indata,BUFFER_Y);

    if ( ret != SUCCESS ) {
        LOGOUT("获取梅尔倒谱系数失败!\n");

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

    float similarity=0;
    float prob_f=0;
    int average,prob_i;
    int count=0;
    for(int time=0;time<xy[num].x;time++){
        count=0;
        similarity=0;
        prob_f=0;
        average=0;
        updateMatrix(num);
        for(int i =0;i<xy[num].x;i++) {
            CSM(realMeil[num][i],dct_meil[num][i],similarity);
            //sum += similarity*10;

            //sum = sum/xy[num].x;
            average = similarity*100;
            //if(average > 70)
#if 0
            if (average < -8 || average >8) {
                string baoluo="|          |          |";
                if(average<0){
                    average=0-average;
                    for(int i=10;11-i<average;i--) baoluo[i]='=';
                } else
                    for(int i=12;i-11<average;i++) baoluo[i]='=';

                //LOGOUT("average %d \n",average);
                LOGOUT("%s %d\n",baoluo.c_str(),count++);
            }
#endif

            //if (average < -8 || average >8) count++;
            //if ( average > 60 ) count++;
            count += average;
        }

        prob_f=count  / xy[num].x;
        prob_i = prob_f;
        if (prob_i >60) {
            LOGOUT("关键字:%s 概率:%2d%% 匹配数:%5d 帧数:%d 匹配次数:%d\n",\
                   xy[num].keyword,prob_i,count,xy[num].x,++xy[num].acount);
        }
    }
    return SUCCESS;
}
