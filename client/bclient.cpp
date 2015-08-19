#include <string.h>
#include <stdio.h>

#include "bclient.h"

using namespace std;

const string BaseClient::LOCAL_HOST = "localhost";

BaseClient::BaseClient() {
    struct hostent *server;

    mServerSock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&mServerAddr, 0, sizeof(sockaddr_in));
    bzero((char *) &mServerAddr, sizeof(mServerAddr));
    mServerAddr.sin_family = AF_INET;
    //mServerAddr.sin_addr.s_addr = INADDR_ANY;
    mServerAddr.sin_port = htons(PORT);

    server = gethostbyname(LOCAL_HOST.c_str());
    if (server == NULL) {
        cerr << "Failed, no such host";
    }
    bcopy((char *)server->h_addr,
            (char *)&mServerAddr.sin_addr.s_addr,
            server->h_length);

    if (connect(mServerSock, (struct sockaddr *)&mServerAddr, sizeof(mServerAddr)) < 0)
        cerr << "ERROR connecting to server";
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

