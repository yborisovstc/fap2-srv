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
	// Key of context map: Handle
	typedef string TCtxKey;
	// Context: key, pointer to iface
	typedef map<TCtxKey, MIface*> TCtx;
    public:
        static vector<SessionClient*> sClients;
        char *mName;
        int mId;
        //Socket stuff
        int mSock;
        SessionThread *mThread;
	//vector<Env*> mEnvs;
	Env* mEnv;
	TCtx mCtx; // Context
	SessionClient* mAttached;

    public:
        SessionClient();
        SessionClient(int sock);
	~SessionClient();
        void Dispatch(int sock);
        void SetName(const char *name);
        void SetId(int id);
        static void * HandleSessionClient(void *args);
	// EnvProvider
	virtual void CreateEnv(const string& aChromo);
	virtual Env* GetEnv(int aEnvId);
	virtual void AttachEnv(int aSessionId, const string& aKey);
	virtual void GetId (string& aSessionId);
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
    private:
//        void HandleMessage(const string& message);
        void HandleMessage(const string& aMsg);
        void Send(string const& msg, const string& msg_args);
        void Send(int msg_id, const string& msg_args);
        void Send(string const& response);
        void Send(const char *message);
        static void ListSessionClients();
        static int FindSessionClientIndex(SessionClient *c);
        static void FindSessionClientById(int mId, SessionClient *&c);
	void AddContext(const string& aHandle, MIface* aPtr);
	MIface* GetContext(const string& aHandle);
	// Debug
	void DumpCtx() const;
	static SessionClient* GetSession(int aId);
};
#endif
