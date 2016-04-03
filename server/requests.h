#ifndef _requests_h_
#define _requesta_h_

#include <string>

using namespace std;
class RequestIPC {
    public:
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
}; 

#endif
