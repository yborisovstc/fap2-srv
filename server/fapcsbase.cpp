#include <errno.h>
#include <stdexcept> 
#include "fapcsbase.h"
#include "requests.h"
#include <stdlib.h>
#include <unistd.h>
#include <ifu.h>
#include <guri.h>
#include <sstream>
#include "../dmas/daaprov.h"
#include "../dmas/mcobspx.h"

using namespace std;



#if 0
AgtObserver::AgtObserver()
{
}


AgtObserver::~AgtObserver()
{
}

string AgtObserver::Mid() const
{
    return string();
}

MIface* AgtObserver::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	    throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	    throw (runtime_error("Wrong arguments number"));
    if (name == "OnCompDeleting") {
	cout << "OnCompDeleting: " << args.at(0);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

void AgtObserver::OnCompDeleting(MElem& aComp, TBool aSoft)
{
}

void AgtObserver::OnCompAdding(MElem& aComp)
{
}

TBool AgtObserver::OnCompChanged(MElem& aComp)
{
    return true;
}

TBool AgtObserver::OnContentChanged(MElem& aComp)
{
    return true;
}

TBool AgtObserver::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    return true;
}

#endif




const int KBufSize = 2048;

EnvProvider::EIfu EnvProvider::mIfu;

// Ifu static initialisation
EnvProvider::EIfu::EIfu()
{
    RegMethod("CreateEnv", 1);
    RegMethod("AttachEnv", 1);
    RegMethod("GetId", 0);
//    RegMethod("CreateAgtObserver", 0);
}

// Base Client session

CSessionBase::TCtx CSessionBase::mSCtx; // Shared Context

//Actually allocate sClients
vector<CSessionBase*> CSessionBase::sClients;

CSessionBase::CSessionBase(): mEnv(NULL), mAttached(NULL)
{
    AddContext("EnvProvider", this);
}

CSessionBase::CSessionBase(int sock): mSock(sock), mEnv(NULL), mAttached(NULL)
{
    AddContext("EnvProvider", this);
}

CSessionBase::~CSessionBase()
{
    // Remove from shared context
    if (mEnv != NULL) {
	RmSContext(mEnv);
	delete mEnv;
    }
}

void CSessionBase::SetId(int id) {
    stringstream ss;
    ss << id;
    mId = ss.str();
}

void CSessionBase::HandleMessage(const string& aMsg) {
    cout << "CSessionBase [" << mId << "] received: " << aMsg << endl;
    size_t ctxid_beg = 0;
    size_t ctxid_end = aMsg.find_first_of(RequestIPC::REQ_SEPARATOR, ctxid_beg); 
    if (ctxid_end == ctxid_beg) {
	Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_CTX_MISSING);
    } else {
	string ctxid = aMsg.substr(ctxid_beg, (ctxid_end == string::npos) ? string::npos : ctxid_end - ctxid_beg);
	// Try local then shared context
	MIface* ctx = GetContext(ctxid);
	if (ctx == NULL) {
	    ctx = GetContext(ctxid, true);
	}
	if (ctx == NULL) {
	    Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_CTX_NOTFOUND);
	    cout << " --> ERR, " << "Ctx not found" << endl;
	} else if (ctxid_end == string::npos) {
	    Send(RequestIPC::RES_ERROR, RequestIPC::RES_ERROR_NOCSPEC);
	    cout << " --> ERR, " << "Ctx spec is missing" << endl;
	} else {
	    size_t cspec_beg = ctxid_end + 1;
	    string cspec = aMsg.substr(cspec_beg);
	    string cres;
	    // Getting the context depending on attachment status
	    MIface* new_ctx = NULL;
	    try {
		new_ctx = ctx->Call(cspec, cres);
	    } catch (exception& e) {
		cout << " --> ERR, " << e.what() << endl;
		Send(RequestIPC::RES_ERROR, e.what());
		return;
	    }
	    if (new_ctx != NULL) {
		string uid = new_ctx->Uid();
		AddContext(uid, new_ctx);
		cout << " --> OK, " << uid << endl;
		Send(RequestIPC::RES_OK, uid);
	    } else {
		cout << " --> OK, " << (cres.empty() ? RequestIPC::RES_OK_NONE : cres) << endl;
		Send(RequestIPC::RES_OK, cres.empty() ? RequestIPC::RES_OK_NONE : cres);
	    }
	}
    }
}

void CSessionBase::Send(string const& msg, const string& msg_args) {
    string response = msg + RequestIPC::REQ_SEPARATOR + msg_args;
    Send(response.c_str());
}

void CSessionBase::Send(string const& aMsg) {
    ssize_t res = send(mSock, aMsg.c_str(), aMsg.size(), 0);
    if (res == -1) {
	cout << "ERROR: " << errno;
    }
}

void CSessionBase::CreateEnv(const string& aChromo)
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
    string uid("MEnv#" + mId);
    // Adding session Id into env variables
    mEnv->SetEVar("SSID", mId);
    // Adding env UID into env variables
    mEnv->SetEVar("EID", uid);
    DaaProv* daaprov = new DaaProv("DaaProv", mEnv);
    // Adding DAA agents provider
    mEnv->AddProvider(daaprov);
    // Adding external objects provider
    CobsPxMgr* cpxmgr = new CobsPxMgr();
    mEnv->SetExtIfProv(cpxmgr);
    //mEnv->ConstructSystem();
    // Adding to contexts
    MEnv* menv = NULL;
    menv = mEnv->GetObj(menv);
    // Adding to shared context
    AddSContext(uid, menv);
}

bool CSessionBase::Run()
{
    char buffer[KBufSize-25];
    int index, n;
    while (1) {
	memset(buffer, 0, sizeof buffer);
	n = recv(mSock, buffer, sizeof buffer, 0);
	//CSessionBase disconnected?
	if(n == 0) {
	    cout << "CSessionBase [" << mId << "] disconnected" << endl;
	    close(mSock);
	    //Remove client in Static sClients <vector> (Critical section!)
	    index = FindSessionIndex(this);
	    cout << "Removed client session, id: " << sClients[index]->mId << endl;
	    sClients.erase(sClients.begin() + index);
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

// Static. Should be called when vector<CSessionBase> sClients is locked!
int CSessionBase::FindSessionIndex(CSessionBase *c) {
    for(size_t i=0; i<sClients.size(); i++) {
	if((sClients[i]->mId) == c->mId) return (int) i;
    }
    cerr << "CSessionBase id not found." << endl;
    return -1;
}

//Static
void CSessionBase::FindSessionById(const string& aId, CSessionBase *&c)
{
    for(size_t i=0; i<sClients.size(); i++) {
	if((sClients[i]->mId) == aId) {
	    c = sClients[i];
	    break;
	}
    }
}

void CSessionBase::AddSContext(const string& aHandle, MIface* aPtr)
{
    TCtxKey key(aHandle);
    if (mSCtx.count(key) > 0) {
	throw(runtime_error("Context already exists"));
    }
    mSCtx.insert(pair<TCtxKey, MIface*>(key, aPtr));
}

void CSessionBase::RmSContext(MIface* aPtr)
{
    bool found = false;
    for (TCtx::iterator it = mSCtx.begin(); it !=mSCtx.end() && !found; it++) {
	if (it->second == aPtr) {
	    found = true;
	    mSCtx.erase(it);
	}
    }
    if (!found) {
	throw(runtime_error("Removing context: not found [" + aPtr->Uid() + "]"));
    }
}

void CSessionBase::AddContext(const string& aHandle, MIface* aPtr)
{
    TCtxKey key(aHandle);
    TCtx& ctx = (mAttached == NULL) ?  mCtx : mAttached->mCtx;
    //assert(ctx.count(key) == 0);
    /*
    if (ctx.count(key) > 0) {
	throw(runtime_error("Context already exists"));
    }
    */
    ctx.insert(pair<TCtxKey, MIface*>(key, aPtr));
}

MIface* CSessionBase::GetContext(const string& aHandle, bool aShared)
{
    MIface* res = NULL;
    TCtxKey key(aHandle);
    TCtx& ctx = aShared ?  mSCtx : ((mAttached == NULL) ?  mCtx : mAttached->mCtx);
    if (ctx.count(key) > 0) {
	res = ctx.at(key);
    }
    return res;
}

void CSessionBase::GetId (string& aSessionId)
{
    stringstream ss;
    ss << mId;
    aSessionId = ss.str();
}

void CSessionBase::AttachEnv(const string& aSessionId)
{
    CSessionBase* session = NULL;
    FindSessionById(aSessionId, session);
    if (session != NULL) {
	mAttached = session;
    } else {
	stringstream ss;
	ss << aSessionId;
	throw(runtime_error("Session [" + ss.str() + "] for attaching not found"));
    }
}

void CSessionBase::DumpCtx() const
{
    for (TCtx::const_iterator it = mCtx.begin(); it != mCtx.end(); it++) {
	cout << it->first << ":" << it->second << "\n";
    }
}

string CSessionBase::Uid() const
{
    return GUriBase::KIfaceSepS + EnvProvider::Type();
}

string CSessionBase::Mid() const
{
    return string();
}

MIface* CSessionBase::Call(const string& aSpec, string& aRes)
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
	aRes = "MEnv#" + mId;
    } else if (name == "GetId") {
	GetId(aRes);
    } else if (name == "AttachEnv") {
	//throw (runtime_error("Unhandled method: " + name));
	AttachEnv(args.at(0));
    } else if (name == "CreateAgtObserver") {
	//CreateAgtObserver();
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

// Static
CSessionBase* CSessionBase::GetSession(const string& aId)
{
    CSessionBase* res = NULL;
    FindSessionById(aId, res);
    return res;
}

/*
void CSessionBase::CreateAgtObserver()
{
    if (mAgtObs != NULL) {
	throw(runtime_error("Agent observer already created"));
    }
    mAgtObs = new AgtObserver();
    if (mAgtObs == NULL) {
	throw(runtime_error("Failed creating agent observer"));
    }
    AddContext(mAgtObs->Uid(), mAgtObs);
}
*/





