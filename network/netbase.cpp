#include "netbase.h"

NetBase::NetBase()
{
    sa=nullptr;
    udpbody=nullptr;
}
NetBase::~NetBase()
{
    if(sa)delete sa;
    if(udpbody)delete udpbody;
}

S_ret NetBase::open(std::string serverip,int port)
{
    sa = new Poco::Net::SocketAddress(serverip.c_str(),port);
    if(sa==nullptr)return FAILED;
    udpbody = new Poco::Net::DatagramSocket(*sa);
    if(udpbody==nullptr)return FAILED;
    return SUCCESS;
}
S_ret NetBase::close()
{
    if(sa)delete sa;
    if(udpbody)delete udpbody;
    sa=nullptr;
    udpbody=nullptr;
}
S_ret NetBase::listen()
{

}
S_ret NetBase::sendData(char *buf,int len)
{

}
S_ret NetBase::recvData(char *buf,int len)
{

}
