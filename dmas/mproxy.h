#ifndef _MPROXY_H_
#define _MPROXY_H_

#include <string>

using namespace std;

/*
 * Proxy interface
 */
class MProxy 
{
    public:
	virtual const string& GetContext() const = 0;
};

/*
 * Proxy manager interface
 */
class MProxyMgr 
{
    public:
	virtual bool Request(const string& aContext, const string& aReq, string& aResp) = 0;
};
#endif
