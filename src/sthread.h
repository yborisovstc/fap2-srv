#ifndef _sessionthread_h_
#define _sessionthread_h_
#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

class SessionThread {
    public:
        pthread_t mTid;
    private:
        static pthread_mutex_t sMutex;
    public:
        SessionThread();
        int Create(void *Callback, void *args);
        int Join();
        static int InitMutex();
        static int LockMutex(const char *identifier);
        static int UnlockMutex(const char *identifier);
};
#endif
