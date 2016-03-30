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
	static const string REQ_INVALID;
	// Request separator
	static const string REQ_SEPARATOR;
	// Response success 
	static const string RES_OK;
	static const string RES_OK_NONE;
	// Response errors
	static const string RES_ERROR;
	static const string RES_ERROR_CTX_NOTFOUND;
	static const string RES_ERROR_CTX_MISSING;
	static const string RES_ERROR_NOCSPEC;
	// Request separator
	static const string R_ARGS_SEPARATOR;
	static const string R_LIST_SEPARATOR;
}; 

#endif
