#ifndef _server_h_
#define _server_h_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>

#define PORT 30666

using namespace std;

class Server {
    public:
        Server();
        Server(int aPort);
	void Construct();
        void AcceptAndDispatch();
    private:
        //Socket stuff
	int mPort;
        int mServerSock, mClientSock;
        struct sockaddr_in mServerAddr, mClientAddr;
};

#endif
