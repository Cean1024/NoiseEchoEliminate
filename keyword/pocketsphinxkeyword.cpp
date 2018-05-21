#include "pocketsphinxkeyword.h"

PocketSphinxKeyword::PocketSphinxKeyword(string hmm, string lm, string dict)
{
    data = nullptr;
    config = cmd_ln_init( NULL, ps_args(), TRUE,\
                          "-hmm", hmm.c_str(),\
                          "-lm", lm.c_str(),\
                          "-dict", dict.c_str(),\
                          NULL);
    if(config== nullptr) {
        LogOut("cmd_ln_init failed");
        return ;
    }
    ps = ps_init(config);
    if(ps == nullptr) {
        LogOut("ps_init failed");
        return ;
    }
    getpcm = nullptr;
    outputresoult = nullptr;
    data = new int16[FRAMESIZE];
    //memcpy(keywords[0],"你好",7);
    memcpy(keywords[1],"你好小桑",13);
    //memcpy(keywords[2],"小桑小桑",13);
    //memcpy(keywords[3],"小桑",7);



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
    if(getpcm == nullptr ) { LogOut("input callback error!!"); return FAILED;}


    int  rv ;

    rv = ps_start_utt(ps);
    if(rv < 0) { LogOut("ps_start_utt error");}
    char * bestwords= "你好小桑";

    int index = 0;
    int size,position ;
    while(true) {

        getpcm(datain,data,FRAMESIZE);
        ps_process_raw(ps, data, FRAMESIZE, FALSE, FALSE);

        rv = ps_get_in_speech(ps);
        if( rv == 1 ) {
            words = ps_get_hyp( ps , nullptr );

            if(words != nullptr   ) {
                size = strlen(words);
                //printf("keyword :%s size:%d rv:%d\n",words,size,rv);
                if( (size - index) > 3 ) {
                    key_word = words;
                    if( (position = key_word.find(bestwords,0,12) ) != string::npos) {
                        ps_end_utt(ps);
                        key_word=key_word.substr(position,12);
                        printf("keyword :%s \n",key_word.c_str());
                        return FOUND;
                    }
                }
                index = size;
            }
        }
    }
    return NOTFOUND;
}
r_status PocketSphinxKeyword::detectKeyword()
{

    int ret;
    KeyWordOutData status;

    while(swith) {
        ret = audio_process( );
        if ( ret == FAILED ) break;
        else if (ret == FOUND ) {
            status.event = HaveKeyWord;
            status.key_word = key_word;
            outputresoult(status , dataout);
        }
    }
}
