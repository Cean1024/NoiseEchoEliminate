#include "linklist.h"

Linklist::Linklist(int blocksize,int maxlenght)
{

    pool=new MemoryPool(blocksize,maxlenght/3,maxlenght);
    list=new MemoryPool(8,maxlenght/3,maxlenght);
    head=nullptr;
    real=nullptr;

}
