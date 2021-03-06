#ifndef _base_client_h_
#define _base_client_h_

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define PORT 30666

using namespace std;

class BaseClient {
    private:
        static const string LOCAL_HOST;
        static const int KBufSize;
        //Socket stuff
        int mServerSock;
        struct sockaddr_in mServerAddr;
        char mBuff[256];
    public:
        BaseClient();
        ~BaseClient();
	void Connect(const string& aHostUri);
	bool Request(const string& aRequest, string& aResponse);
	bool Request(const string& aReqId, const string& aReqArgs, string& aResponse);
        void Dispatch();
	void Disconnect();
};

#endif
