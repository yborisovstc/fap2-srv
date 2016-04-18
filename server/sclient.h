#ifndef _sessionclient_h_
#define _sessionclient_h_

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include "sthread.h"
#include <env.h>
#include <ifu.h>
#include "fapcsbase.h"

using namespace std;


//#if 0
class SessionClient : public CSessionBase {
    public:
        SessionClient();
        SessionClient(int sock);
	virtual ~SessionClient();
        static void* RunSession(void *args);
    protected:
        virtual void Send(string const& aMsg);
        virtual bool Run();
    protected:
        SessionThread *mThread;
};
//#endif



#if 0
class SessionClient : public EnvProvider {
    public:
	// Key of context map: Handle
	typedef string TCtxKey;
	// Context: key, pointer to iface
	typedef map<TCtxKey, MIface*> TCtx;
    public:
	static vector<SessionClient*> sClients;
	string mId;
	//Socket stuff
	int mSock;
        SessionThread *mThread;
	Env* mEnv;
	TCtx mCtx; // Context
	SessionClient* mAttached;
    public:
        SessionClient();
        SessionClient(int sock);
	virtual ~SessionClient();
        static void* HandleSessionClient(void *args);
	// EnvProvider
	virtual void CreateEnv(const string& aChromo);
	virtual void AttachEnv(const string& aSessionId);
	virtual void GetId (string& aSessionId);
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
    protected:
        virtual void HandleMessage(const string& aMsg);
        virtual void Send(string const& aMsg);
        virtual void Run();
        void SetId(int id);
        void Send(string const& msg, const string& msg_args);
        static int FindSessionClientIndex(SessionClient *c);
        static void FindSessionClientById(const string& mId, SessionClient *&c);
	void AddContext(const string& aHandle, MIface* aPtr);
	MIface* GetContext(const string& aHandle);
	// Debug
	void DumpCtx() const;
	static SessionClient* GetSession(const string& aId);
};
#endif

#endif
