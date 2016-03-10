#include "sthread.h"

using namespace std;

//Need to actually "allocate" static member
pthread_mutex_t SessionThread::sMutex;

SessionThread::SessionThread() {
}

int SessionThread::Create(void *Callback, void *args) {
    int res = 0;
    //Supercreepy typecast
    res = pthread_create(&this->mTid, NULL, (void *(*)(void *))Callback, args);
    if (res) {
        cerr << "Error while creating threads." << endl;
        return res;
    }
    else {
        cout << "Thread successfully created." << endl;
        return 0;
    }
}

int SessionThread::Join() {
    pthread_join(this->mTid, NULL);
    return 0;
}

int SessionThread::InitMutex() {
    if (pthread_mutex_init(&SessionThread::sMutex, NULL) < 0) {
        cerr << "Error while initializing mutex" << endl;
        return -1;
    }
    else {
        return 0;
    }
}

/*
   LockMutex():
   Blocks until mutex becomes available
   */
int SessionThread::LockMutex(const char *identifier) {
    //cout << identifier << " is trying to acquire the lock..." << endl;
    if (pthread_mutex_lock(&SessionThread::sMutex) == 0) {
        //cout << identifier << " acquired the lock!" << endl;
        return 0;
    }
    else {
        cerr << "Error while " << identifier << " was trying to acquire the lock" << endl;
        return -1;
    }
}

int SessionThread::UnlockMutex(const char *identifier) {
    //cout << identifier << " is trying to release the lock..." << endl;
    if (pthread_mutex_unlock(&SessionThread::sMutex) == 0) {
        //cout << identifier << " released the lock!" << endl;
        return 0;
    }
    else {
        cerr << "Error while " << identifier << " was trying to release the lock" << endl;
        return -1;
    }
}
