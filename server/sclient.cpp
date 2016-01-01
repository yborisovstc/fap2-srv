#include <stdexcept> 
#include "sclient.h"
#include "requests.h"
#include "rgetmodelslist.h"
#include "rcreateenv.h"
#include "rexec.h"

using namespace std;

//Actually allocate sClients
vector<SessionClient> SessionClient::sClients;

SessionClient::SessionClient(): mEnv(NULL)  {
    mName = (char *) malloc(MAX_NAME_LENGHT+1);
}

SessionClient::SessionClient(int sock): mEnv(NULL) {
    mName = (char *) malloc(MAX_NAME_LENGHT+1);
    Dispatch(sock);
}

void SessionClient::Dispatch(int sock) {
    mSock = sock;
    mThread = new SessionThread();
    mThread->Create((void *) HandleSessionClient, this);
}

void SessionClient::SetName(const char *name) {
    //Copies at most MAX_NAME_LENGHT + 1 (including '\0')
    snprintf(mName, MAX_NAME_LENGHT+1, name);
}

void SessionClient::SetId(int id) {
    mId = id;
}

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
	    HandleMessage(msg_id, msg_args);
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

void SessionClient::HandleMessage(int msg_id, const string& msg_args) {
    cout << "SessionClient, received: " << msg_id << " with args" << msg_args << endl;

    switch (msg_id) {
	case RequestIPC::EReqID_GetModelsList:
	    Send((new ReqGetModelsList(this))->Exec());
	    break;
	case RequestIPC::EReqID_CreateEnv:
	    Send((new ReqCreateEnv(this, msg_args))->Exec());
	    break;
	case RequestIPC::EReqID_Exec:
	    Send((new ReqExec(this, msg_args))->Exec());
	    break;
	case RequestIPC::EReqID_Env:
	    Send((new ReqEnv(this, msg_args))->Exec());
	    break;
	default:
	    Send(msg_id, RequestIPC::RES_ERROR);
	    break;
    }
}

void SessionClient::Send(int msg_id, const string& msg_args) {
    Send(RequestIPC::getResponseCode(msg_id), msg_args);
}

void SessionClient::Send(string const& msg, const string& msg_args) {
    string response = "" + msg + RequestIPC::REQ_SEPARATOR + msg_args;
    Send(response.c_str());
}

void SessionClient::Send(string const& response) {
    Send(response.c_str());
}

void SessionClient::Send(const char *message) {
    int n;
    //Acquire the lock
    SessionThread::LockMutex("'Send()'");
    n = send(mSock, message, strlen(message), 0);
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
    /*
    int id = mEnvs.size();
    string name("Env~");
    stringstream ss;
    ss << id;
    name.append(ss.str());
    cout << "Creating env" << name << endl;
    Env* env = new Env(name, aChromo, false);
    mEnvs.push_back(env);
    env->ConstructSystem();
    */
    
    if (mEnv != NULL) {
	throw(runtime_error("Env already created"));
    }
    string name("Env~");
    stringstream ss;
    ss << mId;
    name.append(ss.str());
    mEnv = new Env(aChromo, name+"log", false);
    mEnv->ConstructSystem();
    if (mEnv != NULL) {
	throw(runtime_error("Failed creating env"));
    }
    // Adding to contexts
    MEnv* menv = mEnv->GetObj(menv);
    AddContext(MEnv::Type(), "0", menv);
}

//Static
void *SessionClient::HandleSessionClient(void *args) {
    //Pointer to accept()'ed SessionClient
    SessionClient *c = (SessionClient *) args;
    char buffer[256-25], message[256];
    int index;
    int n;
    //Add client in Static sClients <vector> (Critical section!)
    SessionThread::LockMutex((const char *) c->mName);
    //Before adding the new client, calculate its id. (Now we have the lock)
    c->SetId(sClients.size());
    sprintf(buffer, "SessionClient n.%d", c->mId);
    c->SetName(buffer);
    cout << "Adding client with id: " << c->mId << endl;
    sClients.push_back(*c);
    SessionThread::UnlockMutex((const char *) c->mName);
    while(1) {
        memset(buffer, 0, sizeof buffer);
        n = recv(c->mSock, buffer, sizeof buffer, 0);
        //SessionClient disconnected?
        if(n == 0) {
            cout << "SessionClient " << c->mName << " diconnected" << endl;
            close(c->mSock);
            //Remove client in Static sClients <vector> (Critical section!)
            SessionThread::LockMutex((const char *) c->mName);
            index = FindSessionClientIndex(c);
            cout << "Erasing user in position " << index << " whose name id is: "
                << sClients[index].mId << endl;
            sClients.erase(sClients.begin() + index);
            SessionThread::UnlockMutex((const char *) c->mName);
            break;
        }
        else if(n < 0) {
            cerr << "Error while receiving message from client: " << c->mName << endl;
        }
        else {
            //Message received.
            //snprintf(message, sizeof message, "<%s>: %s", c->mName, buffer);
            snprintf(message, sizeof message, "%s", buffer);
            c->HandleMessage(string(message));
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
        if((sClients[i].mId) == c->mId) return (int) i;
    }
    cerr << "SessionClient id not found." << endl;
    return -1;
}

void SessionClient::AddContext(const string& aName, const string& aHandle, void* aPtr)
{
    TCtxKey key(aName, aHandle);
    if (mCtx.count(key) > 0) {
	throw(runtime_error("Context already exists"));
    }
    mCtx.insert(pair<TCtxKey, void*>(key, aPtr));
}

void* SessionClient::GetContext(const string& aName, const string& aHandle) const
{
    void* res = NULL;
    TCtxKey key(aName, aHandle);
    if (mCtx.count(key) > 0) {
	res = mCtx.at(key);
    }
    return res;
}

