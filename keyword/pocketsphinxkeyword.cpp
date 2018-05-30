#include "pocketsphinxkeyword.h"
#include <sstream>
PocketSphinxKeyword::PocketSphinxKeyword(string hmm, string lm, string dict)
{
    data = nullptr;
    stringstream itos,iton;
    string samprate;
    string nfftn;

    itos << SAMPLERATE;
    itos >> samprate;

    iton << 2048;
    iton >> nfftn;
    LOGOUT("samprate:%s  nfft: %s ",samprate.c_str(),nfftn.c_str());
    config = cmd_ln_init( NULL, ps_args(), TRUE,\
                          "-nfft",nfftn.c_str(),\
                          "-hmm", hmm.c_str(),\
                          "-lm", lm.c_str(),\
                          "-dict", dict.c_str(),\
                          "-samprate",samprate.c_str(),\
                          NULL);
    if(config== nullptr) {
        LOGOUT("cmd_ln_init failed");
        return ;
    }
    ps = ps_init(config);
    if(ps == nullptr) {
        LOGOUT("ps_init failed");
        return ;
    }
    getpcm = nullptr;
    outputresoult = nullptr;
    data = new int16[FRAMESIZE];
    memcpy(keywords[0],"你好同学",7);
    memcpy(keywords[1],"你好小桑",13);
    memcpy(keywords[2],"小桑小桑",13);
    memcpy(keywords[3],"小桑同学",13);



}

PocketSphinxKeyword::~PocketSphinxKeyword()
{
    if( ps != nullptr ) ps_free(ps);
    if( config != nullptr ) cmd_ln_free_r(config);
    if( data != nullptr ) delete[] data;

}
r_status PocketSphinxKeyword::registeInOut(DataInputCb input,void *datain, \
                                           KeywordOutCb output,void * dataout)
{
    if(input== nullptr || output==nullptr) return FAILED;
    this->getpcm = input;
    this->datain=datain;
    this->outputresoult =output;
    this->dataout=dataout;
    return SUCCESS;

}
r_status PocketSphinxKeyword::audio_process()
{

    char const * words;
    if(getpcm == nullptr ) { LOGOUT("input callback error!!"); return FAILED;}


    int  rv ;
    uint count=0;
    rv = ps_start_utt(ps);
    if(rv < 0) { LOGOUT("ps_start_utt error");}
    //char * bestwords= "你好小桑";
    //char * bestwords = "海建同学";
    //char * bestwords = "小桑同学";
    int index = 0;
    int size,position ;
    while(true) {

        if(getpcm(datain,data,FRAMESIZE) != SUCCESS)LOGOUT("failed to get pcm ");
        ps_process_raw(ps, data, FRAMESIZE, FALSE, FALSE);

        rv = ps_get_in_speech(ps);
        if( rv == 1 ) {
            words = ps_get_hyp( ps , nullptr );
            //LOGOUT("somebody speaking %u",count++);
            if(words != nullptr   ) {
                size = strlen(words);
                printf("keyword :%s size:%d rv:%d\n",words,size,rv);
                if( (size ) > 11 ) {
                    key_word = words;
                    //for(int i=0;i<4;i++) {
                    /*xiaosangtongxue */
                    if( (position = key_word.find(keywords[3],0,12) ) != string::npos) {
                        ps_end_utt(ps);
                        key_word=key_word.substr(position,12);
                        printf("keyword :%s \n",key_word.c_str());
                        return FOUND;
                    }
                    //}
                    //ps_end_utt(ps);
                    //return NOTFOUND;
                }
                if(size != index ) {
                    index = size;
                    count = 0;
                }
                else {
                    count++;
                    if(count >60) {
                        ps_end_utt(ps);
                        return NOTFOUND;
                    }
                }

            }
        }
    }
    return NOTFOUND;
}
r_status PocketSphinxKeyword::detectKeyword()
{
    int ret;
    KeyWordOutData status;

    while(true) {
        ret = audio_process( );
        if ( ret == FAILED ) break;
        else if (ret == FOUND ) {
            status.event = HaveKeyWord;
            status.key_word = key_word;
            outputresoult(status , dataout);
        }
    }
}
