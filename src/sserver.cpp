#include "sserver.h"
using namespace std;

Server::Server() {
    //Initialize static mutex from SessionThread
    SessionThread::InitMutex();
    //For setsock opt (REUSEADDR)
    int yes = 1;
    //Init serverSock and start listen()'ing
    mServerSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&mServerAddr, 0, sizeof(sockaddr_in));
    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_addr.s_addr = INADDR_ANY;
    mServerAddr.sin_port = htons(PORT);
    //Avoid bind error if the socket was not close()'d last time;
    setsockopt(mServerSock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    if (bind(mServerSock, (struct sockaddr *) &mServerAddr, sizeof(sockaddr_in)) < 0)
        cerr << "Failed to bind";
    listen(mServerSock, 5);
}

/*
   AcceptAndDispatch();
   Main loop:
   Blocks at accept(), until a new connection arrives.
   When it happens, create a new thread to handle the new client.
   */
void Server::AcceptAndDispatch() {
    SessionClient *c;
    SessionThread *t;
    socklen_t cliSize = sizeof(sockaddr_in);
    while (1) {
        //Blocks here;
        int sock = accept(mServerSock, (struct sockaddr *) &mClientAddr, &cliSize);
        if (sock < 0) {
            cerr << "Error on accept";
        }
        else {
            c = new SessionClient(sock);
        }
    }
}

