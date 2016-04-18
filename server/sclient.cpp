#include <stdexcept> 
#include "sclient.h"
#include "requests.h"
#include <stdlib.h>
#include <ifu.h>
#include <guri.h>
#include <sstream>
#include "../dmas/daaprov.h"

using namespace std;

const int KBufSize = 2048;

#if 0

//Actually allocate sClients
vector<SessionClient*> SessionClient::sClients;

SessionClient::SessionClient(): mEnv(NULL), mAttached(NULL)  {
    AddContext("EnvProvider", this);
}

SessionClient::SessionClient(int sock): mSock(sock), mEnv(NULL), mAttached(NULL) {
    AddContext("EnvProvider", this);
    mThread = new SessionThread();
    // Adding client in Static sClients registry (Critical section!)
    SessionThread::LockMutex(mId.c_str());
    SetId(sClients.size());
    cout << "Adding client with id: " << mId << endl;
    sClients.push_back(this);
    SessionThread::UnlockMutex(mId.c_str());
    mThread->Create((void *) HandleSessionClient, this);
}

SessionClient::~SessionClient()
{
    if (mEnv != NULL) {
	delete mEnv;
    }
    if (mThread != NULL) {
	delete mThread;
    }
}

void SessionClient::SetId(int id) {
    stringstream ss;
    ss << id;
    mId = ss.str();
}

void SessionClient::HandleMessage(const string& aMsg) {
    cout << "SessionClient [" << mId << "] received: " << aMsg << endl;
    size_t ctxid_beg = 0;
    size_t ctxid_end = aMsg.find_first_of(RequestIPC::REQ_SEPARATOR, ctxid_beg); 
    if (ctxid_end == ctxid_beg) {
	Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_CTX_MISSING);
    } else {
	string ctxid = aMsg.substr(ctxid_beg, (ctxid_end == string::npos) ? string::npos : ctxid_end - ctxid_beg);
	MIface* ctx = GetContext(ctxid);
	if (ctx == NULL) {
	    Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_CTX_NOTFOUND);
	} else if (ctxid_end == string::npos) {
	    Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_NOCSPEC);
	} else {
	    size_t cspec_beg = ctxid_end + 1;
	    string cspec = aMsg.substr(cspec_beg);
	    string cres;
	    // Getting the context depending on attachment status
	    MIface* new_ctx = NULL;
	    try {
		new_ctx = ctx->Call(cspec, cres);
	    } catch (exception& e) {
		Send(RequestIPC::RES_ERROR, e.what());
		return;
	    }
	    if (new_ctx != NULL) {
		string uid = new_ctx->Uid();
		AddContext(uid, new_ctx);
		Send(RequestIPC::RES_OK, uid);
	    } else {
		Send(RequestIPC::RES_OK, cres.empty() ? RequestIPC::RES_OK_NONE : cres);
	    }
	}
    }
}

void SessionClient::Send(string const& msg, const string& msg_args) {
    string response = msg + RequestIPC::REQ_SEPARATOR + msg_args;
    Send(response.c_str());
}

void SessionClient::Send(string const& aMsg) {
    SessionThread::LockMutex("'Send()'");
    send(mSock, aMsg.c_str(), aMsg.size(), 0);
    SessionThread::UnlockMutex("'Send()'");
}

void SessionClient::CreateEnv(const string& aChromo)
{
    if (mEnv != NULL) {
	throw(runtime_error("Env already created"));
    }
    string name("Env~");
    name.append(mId);
    mEnv = new Env(aChromo, name+".log", false);
    if (mEnv == NULL) {
	throw(runtime_error("Failed creating env"));
    }
    // Adding session Id into env variables
    mEnv->SetEVar("SID", mId);
    DaaProv* daaprov = new DaaProv("DaaProv", mEnv);
    mEnv->AddProvider(daaprov);
    //mEnv->ConstructSystem();
    // Adding to contexts
    MEnv* menv = NULL;
    menv = mEnv->GetObj(menv);
    AddContext("MEnv#0", menv);
}

//Static
void *SessionClient::HandleSessionClient(void *args)
{
    SessionClient *c = (SessionClient *) args;
    c->Run();
    //End thread
    return NULL;
}

void SessionClient::Run()
{
    char buffer[KBufSize-25];
    int index, n;
    while (1) {
	memset(buffer, 0, sizeof buffer);
	n = recv(mSock, buffer, sizeof buffer, 0);
	//SessionClient disconnected?
	if(n == 0) {
	    cout << "SessionClient [" << mId << "] disconnected" << endl;
	    close(mSock);
	    //Remove client in Static sClients <vector> (Critical section!)
	    SessionThread::LockMutex(mId.c_str());
	    index = FindSessionClientIndex(this);
	    cout << "Removed client session, id: " << sClients[index]->mId << endl;
	    sClients.erase(sClients.begin() + index);
	    SessionThread::UnlockMutex(mId.c_str());
	    delete this;
	    break;
	} else if (n < 0) {
	    cerr << "Error while receiving message from client: " << mId << endl;
	} else {
	    //Message received.
	    HandleMessage(string(buffer));
	}
    }
}

// Static. Should be called when vector<SessionClient> sClients is locked!
int SessionClient::FindSessionClientIndex(SessionClient *c) {
    for(size_t i=0; i<sClients.size(); i++) {
	if((sClients[i]->mId) == c->mId) return (int) i;
    }
    cerr << "SessionClient id not found." << endl;
    return -1;
}

//Static
void SessionClient::FindSessionClientById(const string& aId, SessionClient *&c)
{
    for(size_t i=0; i<sClients.size(); i++) {
	if((sClients[i]->mId) == aId) {
	    c = sClients[i];
	    break;
	}
    }
}

void SessionClient::AddContext(const string& aHandle, MIface* aPtr)
{
    TCtxKey key(aHandle);
    TCtx& ctx = (mAttached == NULL) ?  mCtx : mAttached->mCtx;
    /*
    if (ctx.count(key) > 0) {
	throw(runtime_error("Context already exists"));
    }
    */
    ctx.insert(pair<TCtxKey, MIface*>(key, aPtr));
}

MIface* SessionClient::GetContext(const string& aHandle)
{
    MIface* res = NULL;
    TCtxKey key(aHandle);
    TCtx& ctx = (mAttached == NULL) ?  mCtx : mAttached->mCtx;
    if (ctx.count(key) > 0) {
	res = ctx.at(key);
    }
    return res;
}

void SessionClient::GetId (string& aSessionId)
{
    stringstream ss;
    ss << mId;
    aSessionId = ss.str();
}

void SessionClient::AttachEnv(const string& aSessionId)
{
    SessionClient* session = NULL;
    FindSessionClientById(aSessionId, session);
    if (session != NULL) {
	mAttached = session;
    } else {
	stringstream ss;
	ss << aSessionId;
	throw(runtime_error("Session [" + ss.str() + "] for attaching not found"));
    }
}

void SessionClient::DumpCtx() const
{
    for (TCtx::const_iterator it = mCtx.begin(); it != mCtx.end(); it++) {
	cout << it->first << ":" << it->second << "\n";
    }
}

string SessionClient::Uid() const
{
    return GUriBase::KIfaceSepS + EnvProvider::Type();
}

string SessionClient::Mid() const
{
    return string();
}

MIface* SessionClient::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = EnvProvider::mIfu.CheckMname(name);
    if (!name_ok)
	throw (runtime_error("Wrong method name"));
    TBool args_ok = EnvProvider::mIfu.CheckMpars(name, args.size());
    if (!args_ok)
	throw (runtime_error("Wrong arguments number"));
    if (name == "CreateEnv") {
	CreateEnv(args.at(0));
	aRes = "MEnv#0";
    } else if (name == "GetId") {
	GetId(aRes);
    } else if (name == "AttachEnv") {
	AttachEnv(args.at(0));
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

// Static
SessionClient* SessionClient::GetSession(const string& aId)
{
    SessionClient* res = NULL;
    FindSessionClientById(aId, res);
    return res;
}

#endif



//#if 0

SessionClient::SessionClient(): CSessionBase()  {
}

SessionClient::SessionClient(int sock): CSessionBase(sock), mThread(NULL)
{
    mThread = new SessionThread();
    // Adding client in Static sClients registry (Critical section!)
    SessionThread::LockMutex(mId.c_str());
    SetId(sClients.size());
    cout << "Adding client with id: " << mId << endl;
    sClients.push_back(this);
    SessionThread::UnlockMutex(mId.c_str());
    mThread->Create((void*) RunSession, this);
}

SessionClient::~SessionClient()
{
    if (mThread != NULL) {
	delete mThread;
    }
}

void SessionClient::Send(string const& aMsg) {
    SessionThread::LockMutex("'Send()'");
    send(mSock, aMsg.c_str(), aMsg.size(), 0);
    SessionThread::UnlockMutex("'Send()'");
}

//Static
void *SessionClient::RunSession(void *args)
{
    SessionClient *c = (SessionClient *) args;
    c->Run();
    //End thread
    return NULL;
}

bool SessionClient::Run()
{
    char buffer[KBufSize-25];
    int index, n;
    while (1) {
	memset(buffer, 0, sizeof buffer);
	n = recv(mSock, buffer, sizeof buffer, 0);
	//SessionClient disconnected?
	if (n == 0) {
	    cout << "SessionClient [" << mId << "] disconnected" << endl;
	    close(mSock);
	    //Remove client in Static sClients <vector> (Critical section!)
	    SessionThread::LockMutex(mId.c_str());
	    index = FindSessionIndex(this);
	    cout << "Removed client session, id: " << sClients[index]->mId << endl;
	    sClients.erase(sClients.begin() + index);
	    SessionThread::UnlockMutex(mId.c_str());
	    delete this;
	    break;
	} else if (n < 0) {
	    cerr << "Error while receiving message from client: " << mId << endl;
	} else {
	    //Message received.
	    HandleMessage(string(buffer));
	}
    }
    return false;
}

//#endif
