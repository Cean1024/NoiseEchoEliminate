#include "netclient.h"

netclient::netclient()
{
    init_client(SERVERIP,PortNumber);
}
netclient::netclient(std::string serverip, int port)
{

    init_client(serverip,port);

}

netclient::~netclient()
{

}


int netclient::init_client(std::string serverip,int port)
{
    //struct sockaddr_in serverid;

    if(clientfd>0)close(clientfd);
    if  (( clientfd = socket ( AF_INET ,  SOCK_STREAM , 0 ) ) < 0 ) {
        perror("LISTEN");
        return FAILED;
    }
    memset(&server_addr, 0 ,sizeof (struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    //strncpy(IP_ADDR,serverip.c_str(),serverip.size());
    //PORT = port;
    server_addr.sin_addr.s_addr = inet_addr( serverip.c_str() );
    server_addr.sin_port = htons( port );
    if ( ( connect ( clientfd ,(struct sockaddr *)&server_addr ,
                     sizeof (struct sockaddr_in) ) ) < 0) {
        LOGOUT("connect error");
        perror("connect ");
        close(clientfd);
        clientfd = -1;
        return FAILED;
    }
    return SUCCESS;

}

void netclient::s_close()
{
    if(clientfd>0)close(clientfd);
    clientfd =-1;
}
int netclient::senddata(char *buf,int size)
{
    if(clientfd<0) return ERROR;
    return send(clientfd , buf , size , 0);
}
