#ifndef _requests_h_
#define _requesta_h_

#include <map>
#include <string>

/*
 * The request structure is: role role_uid action args
 */

using namespace std;
class RequestIPC {
    public:
	typedef map<string, int> TRequestMap;
	static TRequestMap requestMap;
	typedef map<int, string> TResponseMap;
	static TResponseMap responseMap;
	enum {
	    EReqID_Invalid,
	    EReqID_GetModelsList,
	    EReqID_GetModel,
	    EReqID_CreateEnv,
	    EReqID_Exec,
	    EReqID_Env,
	};
	static const string REQ_INVALID;
	static const string REQ_GET_MODELS_LIST;
	static const string REQ_GET_MODEL;
	static const string REQ_CREATE_ENV;
	static const string REQ_EXEC;
	static const string REQ_ENV;
	// Request separator
	static const string REQ_SEPARATOR;
	// Response success 
	static const string RES_OK;
	// Response error 
	static const string RES_ERROR;
	// Request separator
	static const string R_LIST_SEPARATOR;
    public:
	static TRequestMap InitReqMap();
	static TResponseMap InitResMap();
        static int getRequestId(const string& msg);
        static string getResponseCode(int id);
}; 

#endif
