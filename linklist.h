#ifndef LINKLIST_H
#define LINKLIST_H
#include <iostream>
#include <unistd.h>
#include <Poco/MemoryPool.h>
#include <mutex>

#define DBLOCKSIZE 1024

using namespace std;
using namespace Poco;

struct listnode {
    char *data;
    listnode * next;
    short realsize;
};
enum flagstate{
    FLAGLOCKED=1,
    FLAGRELEASE=0
};
class Linklist
{
public:
    Linklist(int blocksize,int maxlenght);
    virtual ~Linklist() {
        delete pool;
        delete list;
    }

    listnode * CreateNode() {
        while(flag == FLAGLOCKED)usleep(100);
        flag =FLAGLOCKED;
        listnode *node;
        try{
            node=(listnode *)list->get();
            node->data=(char *)pool->get();
        } catch (OutOfMemoryException)  {
            node=nullptr;
        }
        flag =FLAGRELEASE;
        return node;
    }
    int InsertNode(listnode *node) {
        while(flag == FLAGLOCKED)usleep(100);
        flag =FLAGLOCKED;
        node->next=nullptr;
        if(head==nullptr && real==nullptr) {
            head=node;
            real=node;

        }else {

            real->next=node;
            real=node;
        }
        flag = FLAGRELEASE;
        return 0;
    }
    listnode * GetNode() {
        listnode *node;
        while(flag == FLAGLOCKED)usleep(100);
        flag =FLAGLOCKED;
        if(head==nullptr && real==nullptr) node = nullptr;
        else{
            node = head;
            head=head->next;

            if(head==nullptr) real=nullptr;
        }
        flag = FLAGRELEASE;
        return node;
    }

    void DestroyNode(listnode *node)
    {
        while(flag == FLAGLOCKED)usleep(100);
        flag =FLAGLOCKED;
        pool->release(node->data);
        list->release(node);
        flag=FLAGRELEASE;
    }

    void Destroylist()
    {
        listnode *node;
        while(1) {
            node=GetNode();
            if(node!=nullptr)DestroyNode(node);
            else break;
        }

    }
private:
    MemoryPool *pool;
    MemoryPool *list;
    listnode *head;
    listnode *real;
    volatile int flag;
};

#endif // LINKLIST_H
