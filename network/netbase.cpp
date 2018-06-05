#include "netbase.h"

NetUdpBase::NetUdpBase()
{
    sa=nullptr;
    udpbody=nullptr;
}
NetUdpBase::~NetUdpBase()
{
    if(sa)delete sa;
    if(udpbody)delete udpbody;
}

S_ret NetUdpBase::open(std::string serverip,int port)
{
    sa = new Poco::Net::SocketAddress(serverip.c_str(),port);
    if(sa==nullptr)return FAILED;
    udpbody = new Poco::Net::DatagramSocket(*sa);
    if(udpbody==nullptr)return FAILED;
    return SUCCESS;
}
S_ret NetUdpBase::close()
{
    if(sa)delete sa;
    if(udpbody)delete udpbody;
    sa=nullptr;
    udpbody=nullptr;
}
S_ret NetUdpBase::listen()
{

}
S_ret NetUdpBase::sendData(char *buf,int len)
{

}
S_ret NetUdpBase::recvData(char *buf,int len)
{
    return udpbody->receiveBytes(buf,len);
}
