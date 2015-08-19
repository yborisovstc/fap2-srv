#ifndef _sessionclient_h_
#define _sessionclient_h_
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "sthread.h"
#include "reqbase.h"

#define MAX_NAME_LENGHT 20

using namespace std;
class SessionClient : public ReqBase::EnvProvider {
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
	// EnvProvider
	void getEnv();
    private:
        void HandleMessage(const string& message);
        void HandleMessage(int msg_id, const string& msg_args);
        void Send(string const& msg, const string& msg_args);
        void Send(int msg_id, const string& msg_args);
        void Send(string const& response);
        void Send(const char *message);
        static void ListSessionClients();
        static int FindSessionClientIndex(SessionClient *c);
};
#endif
