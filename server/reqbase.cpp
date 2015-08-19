#include "reqbase.h"
#include "requests.h"

ReqBase::ReqBase(int id, EnvProvider *envProv) {
    mId = id;
    mEnvProvider = envProv;
}

ReqBase::ReqBase(int id, EnvProvider *envProv, string args) {
    mId = id;
    mEnvProvider = envProv;
    mArgs = args;
}

string ReqBase::Exec() {
    ParseArgs();
    SetRes(Execute());
    return GetRes();
}

void ReqBase::SetRes(string res) {
    mRes = res;
}

string ReqBase::GetRes() {
    string response = "" + RequestIPC::getResponseCode(mId) + RequestIPC::REQ_SEPARATOR + mRes;
    return response;
}


