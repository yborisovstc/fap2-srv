#include <stdexcept> 
#include "sclient.h"
#include "requests.h"
#include "rgetmodelslist.h"
#include "rcreateenv.h"
#include "rexec.h"
#include <stdlib.h>
#include <time.h>
#include <ifu.h>
#include <guri.h>
#include <sstream>
#include "../dmas/daaprov.h"

using namespace std;

const int KBufSize = 1024;

//Actually allocate sClients
vector<SessionClient*> SessionClient::sClients;

SessionClient::SessionClient(): mEnv(NULL), mAttached(NULL)  {
    mName = (char *) malloc(MAX_NAME_LENGHT+1);
    srand(time(NULL));
    AddContext("EnvProvider", this);
}

SessionClient::SessionClient(int sock): mEnv(NULL), mAttached(NULL) {
    mName = (char *) malloc(MAX_NAME_LENGHT+1);
    Dispatch(sock);
    srand(time(NULL));
    AddContext("EnvProvider", this);
}

SessionClient::~SessionClient()
{
    if (mEnv != NULL) {
	delete mEnv;
    }
    if (mThread != NULL) {
	delete mThread;
    }
    if (mName != NULL) {
	delete mName;
    }
}

void SessionClient::Dispatch(int sock) {
    mSock = sock;
    mThread = new SessionThread();
    mThread->Create((void *) HandleSessionClient, this);
}

void SessionClient::SetName(const char *name) {
    //Copies at most MAX_NAME_LENGHT + 1 (including '\0')
    snprintf(mName, MAX_NAME_LENGHT+1, "%s", name);
}

void SessionClient::SetId(int id) {
    mId = id;
}

#if 0
void SessionClient::HandleMessage(const string& message) {
    cout << "SessionClient " << mName << " received msg: " << message << endl;

    bool handled = false;
    size_t msg_end = message.find_first_of('\n', 0); 
    string msg = message.substr(0, (msg_end == string::npos) ? message.length() : msg_end);
    msg_end = msg.find_first_of(RequestIPC::REQ_SEPARATOR, 0); 
    if (msg_end > 0) {
	msg = message.substr(0, (msg_end == string::npos) ? msg.length() : msg_end);
	int msg_id = RequestIPC::getRequestId(msg);
	if (msg_id != RequestIPC::EReqID_Invalid) {
	    handled = true;
	    string msg_args = (msg_end == string::npos) ? "" : message.substr(msg_end + 1);
	    HandleMessage(msg, msg_args);
	} else {
	    cout << "SessionClient, msg code not found, msg=" << msg << endl;
	}
    } else {
	cout << "SessionClient, wrong message" << endl;
    }

    if (!handled) {
    // send error
    Send(message, RequestIPC::RES_ERROR);
    }
}
#endif

void SessionClient::HandleMessage(const string& aMsg) {
    cout << "SessionClient " << mName << " received msg: " << aMsg << endl;

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
		//stringstream ss;
		//ss << rand();
		//AddContext(ss.str(), new_ctx);
		string uid = new_ctx->Uid();
		AddContext(uid, new_ctx);
		Send(RequestIPC::RES_OK, uid);
	    } else {
		Send(RequestIPC::RES_OK, cres.empty() ? "OK":cres);
	    }
	}
    }
}

void SessionClient::Send(int msg_id, const string& msg_args) {
    Send(RequestIPC::getResponseCode(msg_id), msg_args);
}

void SessionClient::Send(string const& msg, const string& msg_args) {
    //string response = "" + msg + RequestIPC::REQ_SEPARATOR + msg_args;
    string response = msg + " " + msg_args;
    Send(response.c_str());
}

void SessionClient::Send(string const& response) {
    Send(response.c_str());
}

void SessionClient::Send(const char *message) {
    //Acquire the lock
    SessionThread::LockMutex("'Send()'");
    send(mSock, message, strlen(message), 0);
    //Release the lock
    SessionThread::UnlockMutex("'Send()'");
}


// EnvProvider
Env* SessionClient::GetEnv(int aEnvId)
{
    /*
       Env* res = NULL;
       if (aEnvId > 0 && aEnvId < mEnvs.size()) {
       res = mEnvs.at(aEnvId);
       }
       return res;
       */
    return mEnv;
}

void SessionClient::CreateEnv(const string& aChromo)
{
    if (mEnv != NULL) {
	throw(runtime_error("Env already created"));
    }
    string name("Env~");
    stringstream ss;
    ss << mId;
    string sid = ss.str();
    name.append(sid);
    mEnv = new Env(aChromo, name+".log", false);
    if (mEnv == NULL) {
	throw(runtime_error("Failed creating env"));
    }
    // Adding session Id into env variables
    mEnv->SetEVar("EnvId", sid);
    DaaProv* daaprov = new DaaProv("DaaProv", mEnv);
    mEnv->AddProvider(daaprov);
    //mEnv->ConstructSystem();
    // Adding to contexts
    MEnv* menv = NULL;
    menv = mEnv->GetObj(menv);
    AddContext("MEnv#0", menv);
}

//Static
void *SessionClient::HandleSessionClient(void *args) {
    //Pointer to accept()'ed SessionClient
    SessionClient *c = (SessionClient *) args;
    char buffer[KBufSize-25], message[KBufSize];
    int index;
    int n;
    //Add client in Static sClients <vector> (Critical section!)
    SessionThread::LockMutex((const char *) c->mName);
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(sClients.size());
    sprintf(buffer, "SessionClient n.%d", c->mId);
    c->SetName(buffer);
    cout << "Adding client with id: " << c->mId << endl;
    sClients.push_back(c);
    SessionThread::UnlockMutex((const char *) c->mName);
    while(1) {
	memset(buffer, 0, sizeof buffer);
	n = recv(c->mSock, buffer, sizeof buffer, 0);
	//SessionClient disconnected?
	if(n == 0) {
	    cout << "SessionClient " << c->mName << " disconnected" << endl;
	    close(c->mSock);
	    //Remove client in Static sClients <vector> (Critical section!)
	    SessionThread::LockMutex((const char *) c->mName);
	    index = FindSessionClientIndex(c);
	    cout << "Erasing user in position " << index << " whose name id is: "
		<< sClients[index]->mId << endl;
	    sClients.erase(sClients.begin() + index);
	    SessionThread::UnlockMutex((const char *) c->mName);
	    delete c;
	    break;
	}
	else if(n < 0) {
	    cerr << "Error while receiving message from client: " << c->mName << endl;
	}
	else {
	    //Message received.
	    //snprintf(message, sizeof message, "<%s>: %s", c->mName, buffer);
	    //snprintf(message, sizeof message, "%s", buffer);
	    c->HandleMessage(string(buffer));
	}
    }
    //End thread
    return NULL;
}

/*
   Should be called when vector<SessionClient> sClients is locked!
   */
//Static
int SessionClient::FindSessionClientIndex(SessionClient *c) {
    for(size_t i=0; i<sClients.size(); i++) {
	if((sClients[i]->mId) == c->mId) return (int) i;
    }
    cerr << "SessionClient id not found." << endl;
    return -1;
}

//Static
void SessionClient::FindSessionClientById(int aId, SessionClient *&c)
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

void SessionClient::AttachEnv(int aSessionId, const string& aKey)
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

MIface* SessionClient::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    if (name == "GetEnv") {
	Env* env = GetEnv(0);
	if (env != NULL) {
	    aRes = "MEnv#0";
	}
    } else if (name == "CreateEnv") {
	if (args.size() != 1) 
	    throw (runtime_error("Wrong arguments number"));
	CreateEnv(args.at(0));
	aRes = "MEnv#0";
    } else if (name == "GetId") {
	if (args.size() != 0) 
	    throw (runtime_error("Wrong arguments number"));
	GetId(aRes);
    } else if (name == "AttachEnv") {
	if (args.size() != 1) 
	    throw (runtime_error("Wrong arguments number"));
	stringstream ss(args.at(0));
	int id;
	ss >> id;
	AttachEnv(id, "");
    } else {
	throw (runtime_error("Unknown method"));
    }
    return res;
}

// Static
SessionClient* SessionClient::GetSession(int aId)
{
    SessionClient* res = NULL;
    FindSessionClientById(aId, res);
    return res;
    
}
