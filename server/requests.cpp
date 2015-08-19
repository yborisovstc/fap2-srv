#include "requests.h"

const string RequestIPC::REQ_INVALID = "req_invalid";
const string RequestIPC::REQ_GET_MODEL_LIST = "get_model_list";
const string RequestIPC::REQ_GET_MODEL = "get_model";

const string RequestIPC::REQ_SEPARATOR = " ";
const string RequestIPC::R_LIST_SEPARATOR = ";";

const string RequestIPC::RES_OK = "ok";
const string RequestIPC::RES_ERROR = "error";

RequestIPC::TRequestMap RequestIPC::InitReqMap() { 
    TRequestMap rmap;
    rmap[REQ_GET_MODEL_LIST] = EReqID_GetModelsList;
    rmap[REQ_GET_MODEL] = EReqID_GetModel;
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



