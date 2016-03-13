#ifndef _DAAPROXY_h_
#define _DAAPROXY_h_

#include "mproxy.h"
#include "menv.h"
#include "mlog.h"
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
	// From MIface	
	virtual string Uid() const;
	virtual string Mid() const;
    public:
	virtual DaaProxy* CreateProxy(const string& aId, MProxyMgr* aMgr, const string& aContext) const;
	virtual void *GetIface(const string& aName);
	virtual const void *GetIface(const string& aName) const;
    protected:
	TBool IsCached(const string& aContext) const;
	void RegProxy(DaaProxy* aProxy);
	DaaProxy* GetProxy(const string& aContext) const;
	inline MProvider* Provider() const;
	inline MLogRec* Logger() const;
	void* NewProxyRequest(const string& aCallSpec, const string& aPxType);
	const void* NewProxyRequest(const string& aCallSpec, const string& aPxType) const;
    protected:
	MEnv* mEnv;
	MProxyMgr* mMgr;
	string mContext;
	map<string, DaaProxy*> mProxies;
};

inline MProvider* DaaProxy::Provider() const {return mEnv ? mEnv->Provider(): NULL; }

inline MLogRec* DaaProxy::Logger() const {return mEnv ? mEnv->Logger(): NULL; }

#endif 
