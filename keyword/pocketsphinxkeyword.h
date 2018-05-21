#ifndef POCKETSPHINXKEYWORD_H
#define POCKETSPHINXKEYWORD_H
#include "common.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <pocketsphinx/pocketsphinx.h>


using namespace std;
class PocketSphinxKeyword
{
public:
    PocketSphinxKeyword(string hmm,string lm ,string dict);
    ~PocketSphinxKeyword();
    r_status registeInOut(DataInputCb input,void *datain, \
                          KeywordOutCb output,void * dataout);
    r_status audio_process();
    r_status detectKeyword();

private:
    cmd_ln_t *config;
    ps_decoder_t *ps;
    DataInputCb getpcm;
    void *datain;
    KeywordOutCb outputresoult;
    void *dataout;
    int16 *data;
    string key_word;

    char keywords[NUMKEIWORDS][20];
};

#endif // POCKETSPHINXKEYWORD_H
