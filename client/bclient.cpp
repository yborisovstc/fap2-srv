#include <string.h>
#include <stdio.h>
#include <stdexcept> 
#include <assert.h>
#include "../server/requests.h" 

#include "bclient.h"

using namespace std;

const string BaseClient::LOCAL_HOST = "localhost";
const int BaseClient::KBufSize = 512;

BaseClient::BaseClient(): mState(St_Idle)
{

    mServerSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&mServerAddr, 0, sizeof(sockaddr_in));
    bzero((char *) &mServerAddr, sizeof(mServerAddr));
    mServerAddr.sin_family = AF_INET;
    //mServerAddr.sin_addr.s_addr = INADDR_ANY;
    mServerAddr.sin_port = htons(PORT);
    pthread_mutex_init(&mMutex, NULL);
}

BaseClient::~BaseClient()
{
    if (mServerSock != 0) {
	Disconnect();
    }
}

void BaseClient::Connect(const string& aHostUri)
{
    struct hostent *server;

    server = gethostbyname(LOCAL_HOST.c_str());
    if (server == NULL) {
	throw(runtime_error("Cannot get the host"));
    }
    bcopy((char *)server->h_addr,
            (char *)&mServerAddr.sin_addr.s_addr,
            server->h_length);

    if (connect(mServerSock, (struct sockaddr *)&mServerAddr, sizeof(mServerAddr)) < 0) {
	throw(runtime_error("Error connecting to server"));
    }
    pthread_mutex_lock(&mMutex);
    mState = St_Ready;
    pthread_mutex_unlock(&mMutex);
}

bool BaseClient::Request(const string& aRequest, string& aResponse)
{
    bool is_ready = false;
    pthread_mutex_lock(&mMutex);
    is_ready = (mState == St_Ready);
    pthread_mutex_unlock(&mMutex);
    assert(is_ready);
    bool res = true;
    int n = send(mServerSock, aRequest.c_str(), aRequest.size(), 0);
    if (n < 0) {
	//throw(runtime_error("ERROR writing to socket"));
	res = false;
	aResponse = "Error writing to socket";
	return res;
    }
    pthread_mutex_lock(&mMutex);
    mState = St_Requesting;
    pthread_mutex_unlock(&mMutex);
    char buffer[KBufSize];
    bzero(buffer, KBufSize);
    n = recv(mServerSock, buffer, sizeof buffer, 0);
    if (n < 0) {
	//throw(runtime_error("ERROR reading from socket"));
	res = false;
	aResponse = "Error reading from socket";
	return res;
    }
    aResponse.assign(buffer + 2, n - 2);
    res = (buffer[0] != 'E');
    pthread_mutex_lock(&mMutex);
    mState = St_Ready;
    pthread_mutex_unlock(&mMutex);
    return res;
}

bool BaseClient::Request(const string& aReqId, const string& aReqArgs, string& aResponse)
{
    string req(aReqId);
    req.append(RequestIPC::REQ_SEPARATOR);
    req.append(aReqArgs);
    return Request(req, aResponse);
}
/*
   Dispatch();
   Main loop:
   */
void BaseClient::Dispatch() {
    char buffer[256];
    int n;
    while (1) {
        printf("Please enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = send(mServerSock, buffer, strlen(buffer), 0);
        if (n < 0)
            cerr << "ERROR writing to socket";
        bzero(buffer, 256);
        n = recv(mServerSock, buffer, sizeof buffer, 0);
        if (n < 0)
            cerr << "ERROR reading from socket";
        printf("%s\n",buffer);
    }
}

void BaseClient::Disconnect()
{
    close(mServerSock);
    mServerSock = 0;
}

bool BaseClient::IsReady()
{
    bool res = false;
    pthread_mutex_lock(&mMutex);
    res =  mState == St_Ready;
    pthread_mutex_unlock(&mMutex);
    return res;
}

