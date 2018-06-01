#ifndef NETBASE_H
#define NETBASE_H

#include "common.h"
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

class NetBase
{
public:
    NetBase();
    virtual ~NetBase();
    S_ret open(std::string serverip,int port);
    S_ret close();
    S_ret listen();
    S_ret sendData(char *buf,int len);
    S_ret recvData(char *buf,int len);

private:
    Poco::Net::SocketAddress *sa;
    Poco::Net::DatagramSocket *udpbody;

};

#endif // NETBASE_H
