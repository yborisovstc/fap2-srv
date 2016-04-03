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

using namespace std;

class EnvProvider: public MIface {
    public:
	static const char* Type() { return "EnvProvider";};
	virtual void CreateEnv(const string& aChromo) = 0;
	virtual void AttachEnv(const string& aSessionId) = 0;
	virtual void GetId (string& aSessionId) = 0;
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;

};

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
	~SessionClient();
        void Dispatch(int sock);
        void SetId(int id);
        static void * HandleSessionClient(void *args);
	// EnvProvider
	virtual void CreateEnv(const string& aChromo);
	virtual void AttachEnv(const string& aSessionId);
	virtual void GetId (string& aSessionId);
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
    private:
        void HandleMessage(const string& aMsg);
        void Send(string const& msg, const string& msg_args);
        void Send(string const& aMsg);
        static int FindSessionClientIndex(SessionClient *c);
        static void FindSessionClientById(const string& mId, SessionClient *&c);
	void AddContext(const string& aHandle, MIface* aPtr);
	MIface* GetContext(const string& aHandle);
	// Debug
	void DumpCtx() const;
	static SessionClient* GetSession(const string& aId);
};
#endif
