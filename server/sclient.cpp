#include "sclient.h"
#include "requests.h"
#include "rgetmodelslist.h"

using namespace std;

//Actually allocate sClients
vector<SessionClient> SessionClient::sClients;

SessionClient::SessionClient() {
    mName = (char *) malloc(MAX_NAME_LENGHT+1);
}

SessionClient::SessionClient(int sock) {
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
void SessionClient::getEnv() {
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

