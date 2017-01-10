#include <string.h>
#include <stdio.h>
#include <stdexcept> 
#include <assert.h>
#include <sstream>
#include "../server/requests.h" 
#include <unistd.h>

#include "bclient.h"

using namespace std;

const string BaseClient::LOCAL_HOST = "localhost";
const int BaseClient::KBufSize = 2048;
const char KSchemeSep = ':';
const char KPathDelim = '/';
const char KHostDelim = ':';

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
    string scheme, host, port;

    ParseUri(aHostUri, scheme, host, port);
    if (host.empty()) {
	host = LOCAL_HOST;
    }
    server = gethostbyname(host.c_str());
    if (server == NULL) {
	throw(runtime_error("Cannot get the host"));
    }
    bcopy((char *)server->h_addr, (char *)&mServerAddr.sin_addr.s_addr, server->h_length);
    int porti = port.empty() ? PORT : GetPortInt(port);
    mServerAddr.sin_port = htons(porti);
    if (connect(mServerSock, (struct sockaddr *)&mServerAddr, sizeof(mServerAddr)) < 0) {
	throw(runtime_error("Error connecting to server"));
    }
    pthread_mutex_lock(&mMutex);
    mState = St_Ready;
    pthread_mutex_unlock(&mMutex);
}

// Static
int BaseClient::ParseUri(const string& aUri, string& aScheme, string& aHost, string& aPort)
{
    size_t scheme_end = aUri.find_first_of(KSchemeSep, 0);
    if (scheme_end != string::npos) {
	aScheme = aUri.substr(0, scheme_end);
    }
    size_t auth_beg = (scheme_end != string::npos) ? scheme_end + 1 : 0;
    size_t auth_end = auth_beg;
    string auth;
    if ((aUri.size() > (scheme_end + 2)) && (aUri.at(scheme_end + 1) == KPathDelim)
	    && (aUri.at(scheme_end + 2) == KPathDelim)) {
	auth_beg = auth_beg + 2;
	auth_end = aUri.find_first_of(KPathDelim, auth_beg);
	auth = aUri.substr(auth_beg, (auth_end == string::npos) ? string::npos : auth_end - auth_beg);
    }
    size_t host_beg = 0;
    size_t host_end = auth.find_first_of(KHostDelim, 0);
    aHost = auth.substr(host_beg, (host_end == string::npos) ? string::npos : host_end - host_beg);
    if (host_end != string::npos) {
	size_t port_beg = host_end + 1;
	size_t port_end = string::npos;
	aPort = auth.substr(port_beg, (port_end == string::npos) ? string::npos : port_end - port_beg);
    }
    return 0;
}

int BaseClient::GetPortInt(const string& aPort)
{
    int res;
    istringstream ss(aPort);
    ss >> res;
    return res;
}

bool BaseClient::Request(const string& aRequest, string& aResponse)
{
    bool is_ready = false;
    pthread_mutex_lock(&mMutex);
    is_ready = (mState == St_Ready);
    assert(is_ready);
    mState = St_Requesting;
    pthread_mutex_unlock(&mMutex);
    bool res = true;
    int n = send(mServerSock, aRequest.c_str(), aRequest.size(), 0);
    if (n < 0) {
	//throw(runtime_error("ERROR writing to socket"));
	res = false;
	aResponse = "Error writing to socket";
	return res;
    }
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

bool BaseClient::IsConnected()
{
    bool res = false;
    pthread_mutex_lock(&mMutex);
    res =  mState != St_Idle;
    pthread_mutex_unlock(&mMutex);
    return res;
}

