#ifndef _MPROXY_H_
#define _MPROXY_H_

#include <string>

using namespace std;

class MIface;

/*
 * Proxy interface
 */
class MProxy 
{
    public:
	virtual const string& GetContext() const = 0;
	virtual MIface* GetIface(const string& aName) = 0;
	virtual const MIface* GetIface(const string& aName) const = 0;
};

/*
 * Proxy manager interface
 */
class MProxyMgr 
{
    public:
	virtual MProxy* CreateProxy(const string& aId, const string& aContext) = 0;
	virtual bool Request(const string& aContext, const string& aReq, string& aResp) = 0;
	// Object Id
	virtual string Oid() const = 0;
	virtual void OnProxyDeleting(const MProxy* aProxy) = 0;
};
#endif
