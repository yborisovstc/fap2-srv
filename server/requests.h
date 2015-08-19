#ifndef _requests_h_
#define _requesta_h_

#include <map>
#include <string>

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
	    EReqID_GetModel
	};
	static const string REQ_INVALID;
	static const string REQ_GET_MODEL_LIST;
	static const string REQ_GET_MODEL;
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
