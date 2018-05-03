#ifndef KEYWORDDETECT_H
#define KEYWORDDETECT_H

#include "common.h"
#include <iostream>
#include <fstream>
#include <string.h>

#define READLEN 200

using namespace std;
class keyword{
public:
    r_status readmel_coe(string filepath){

        ifstream fp(filepath,ios::in|ios::binary);
        char buf[READLEN];

        while(!fp.eof()) {
            memset(buf,0,READLEN);
            fp.getline(buf,READLEN);
            cout << buf <<endl ;
        }
        cout << endl;
        fp.close();
        return SUCCESS;
    }




};

class KeyWordDetect:public keyword
{
public:
    KeyWordDetect();
    KeyWordDetect(string filepath)
    {
        readmel_coe(filepath);

    }
};

#endif // KEYWORDDETECT_H
