#ifndef NETCLIENT_H
#define NETCLIENT_H
#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

#define PortNumber 9009

#define SERVERIP "192.168.199.245"

class netclient
{
public:
    netclient();
    netclient(std::string serverip, int port);
    ~netclient();


    int senddata(char *buf,int size);
    void s_close();

protected:
    int init_client(std::string serverip, int port);
private:
    int ret;
    int sockfd;

    int clientfd;

    struct sockaddr_in server_addr;
    struct hostent * host;
};
#endif // NETCLIENT_H
