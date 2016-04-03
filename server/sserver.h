#ifndef _server_h_
#define _server_h_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include "sclient.h"

#define PORT 30666

using namespace std;

class Server {
    public:
        Server();
        void AcceptAndDispatch();
    private:
        //Socket stuff
        int mServerSock, mClientSock;
        struct sockaddr_in mServerAddr, mClientAddr;
};

#endif
