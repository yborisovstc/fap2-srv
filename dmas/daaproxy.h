#ifndef _DAAPROXY_h_
#define _DAAPROXY_h_

#include "mproxy.h"
#include "menv.h"
#include <map>

/*
 * Proxy base
 */

class DaaProxy : public MProxy, public MProxyMgr
{
    public:
	DaaProxy(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~DaaProxy();
	// From MProxy
	virtual const string& GetContext() const;
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
    protected:
	TBool IsCached(const string& aContext) const;
	void RegProxy(DaaProxy* aProxy);
	DaaProxy* GetProxy(const string& aContext);
    protected:
	MEnv* mEnv;
	MProxyMgr* mMgr;
	string mContext;
	map<string, DaaProxy*> mProxies;
};

#endif 
