#ifndef _sessionclient_h_
#define _sessionclient_h_
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "sthread.h"

#define MAX_NAME_LENGHT 20

using namespace std;
class SessionClient {
    public:
        static vector<SessionClient> sClients;
        char *mName;
        int mId;
        //Socket stuff
        int mSock;
        SessionThread *mThread;
    public:
        SessionClient();
        SessionClient(int sock);
        void Dispatch(int sock);
        void SetName(const char *name);
        void SetId(int id);
        static void * HandleSessionClient(void *args);
    private:
        void HandleMessage(char *message);
        void Send(char *message);
        static void ListSessionClients();
        static int FindSessionClientIndex(SessionClient *c);
};
#endif
