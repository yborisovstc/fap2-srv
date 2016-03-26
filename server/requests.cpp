#include "requests.h"

const string RequestIPC::REQ_INVALID = "req_invalid";
const string RequestIPC::REQ_GET_MODELS_LIST = "get_models_list";
const string RequestIPC::REQ_GET_MODEL = "get_model";
const string RequestIPC::REQ_CREATE_ENV = "cenv";
const string RequestIPC::REQ_EXEC = "exec";

const string RequestIPC::REQ_SEPARATOR = " ";
const string RequestIPC::R_ARGS_SEPARATOR = ",";
const string RequestIPC::R_LIST_SEPARATOR = ";";

const string RequestIPC::RES_OK = "O";
const string RequestIPC::RES_OK_NONE = "[NONE]";
const string RequestIPC::RES_ERROR = "E";
const string RequestIPC::RES_ERROR_CTX_NOTFOUND = "Context not found";
const string RequestIPC::RES_ERROR_CTX_MISSING = "Missing context";
const string RequestIPC::RES_ERROR_NOCSPEC = "Missing call spec";

RequestIPC::TRequestMap RequestIPC::InitReqMap() { 
    TRequestMap rmap;
    rmap[REQ_GET_MODELS_LIST] = EReqID_GetModelsList;
    rmap[REQ_GET_MODEL] = EReqID_GetModel;
    rmap[REQ_CREATE_ENV] = EReqID_CreateEnv;
    rmap[REQ_EXEC] = EReqID_Exec;
    return rmap;
};

RequestIPC::TRequestMap RequestIPC::requestMap = RequestIPC::InitReqMap(); 

RequestIPC::TResponseMap RequestIPC::InitResMap() { 
    TResponseMap rmap;
    for (RequestIPC::TRequestMap::iterator it = requestMap.begin(); it != requestMap.end(); it++) {
	rmap[it->second] = it->first;
    }
    return rmap;
};

RequestIPC::TResponseMap RequestIPC::responseMap = RequestIPC::InitResMap(); 

int RequestIPC::getRequestId(const string& msg) { 
    int res = EReqID_Invalid;
    if (requestMap.count(msg) > 0) {
       res = requestMap.at(msg);
    }
    return res;
}

string RequestIPC::getResponseCode(int id) {
    string res = REQ_INVALID;
    if (responseMap.count(id) > 0) {
       res = responseMap.at(id);
    }
    return res;
}



