#include "sclient.h"
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

void SessionClient::HandleMessage(char *message) {
    cout << "SessionClient " << mName << " received msg:" << message << endl;
    // send reply
    Send(message);
}

void SessionClient::Send(char *message) {
    int n;
    //Acquire the lock
    SessionThread::LockMutex("'Send()'");
    n = send(mSock, message, strlen(message), 0);
    //Release the lock
    SessionThread::UnlockMutex("'Send()'");
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
            snprintf(message, sizeof message, "<%s>: %s", c->mName, buffer);
            c->HandleMessage(message);
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

