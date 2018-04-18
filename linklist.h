#ifndef LINKLIST_H
#define LINKLIST_H
#include <iostream>
#include <Poco/MemoryPool.h>
#define DBLOCKSIZE 1024
using namespace std;
using namespace Poco;

struct listnode {
    char *data;
    listnode * next;
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
        listnode *node;
        try{
            node=(listnode *)list->get();
            node->data=(char *)pool->get();
        } catch (OutOfMemoryException)  {
            node=nullptr;
        }
        return node;
    }
    int InsertNode(listnode *node) {
        if(head==nullptr && real==nullptr){
            head=node;
            real=node;
            head->next=nullptr;
            real->next=nullptr;
        }else{
            node->next=nullptr;
            real->next=node;
            real=node;
        }
        return 0;
    }
    listnode * GetNode() {
        listnode *node;
        if(head==nullptr && real==nullptr) node = nullptr;
        else{
            node = head;
            if(head!=real) head=head->next;
            else {head=nullptr ;real=nullptr;}
        }

        return node;
    }

    void DestroyNode(listnode *node)
    {
        pool->release(node->data);
        list->release(node);
    }

    void Destroylist()
    {
        listnode *node;
        while(1){
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
};

#endif // LINKLIST_H
