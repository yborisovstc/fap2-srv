#include <dirent.h>
#include <sys/stat.h>

#include "rcreateenv.h"
#include "requests.h"


ReqCreateEnv::ReqCreateEnv(EnvProvider *envProv, const string& aChromo) : ReqBase(RequestIPC::EReqID_CreateEnv, envProv),
    mChromo(aChromo)
{
}

string ReqCreateEnv::Execute()
{
    mEnvProvider->CreateEnv(mChromo);
    return "OK";
}


