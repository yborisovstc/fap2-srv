#ifndef _mcobspx_h_
#define _mcobspx_h_

#include <menv.h>
#include <melem.h>
#include "daaproxy.h"
#include "../client/bclient.h"


/*
 * External interface proxy base class
 */
class EIfacePx
{
    public:
	// TODO [YB] Having env in external object proxy seems incorrect, to redesign
	EIfacePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~EIfacePx();
	inline MLogRec* Logger() const;
	virtual MIface* GetIface(const string& aName) = 0;
	virtual const MIface* GetIface(const string& aName) const = 0;
	const string& GetContext() const;
    protected:
	MEnv* mEnv;
	MProxyMgr* mMgr;
	string mContext;
};

inline MLogRec* EIfacePx::Logger() const {return mEnv ? mEnv->Logger(): NULL; }

/*
 * Components observer proxy
 */
class MCobsPx: public DaaProxy, public MAgentObserver
{
    public:
	MCobsPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MCobsPx();
    public:
	// From EIfacePx
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MCompsObserver
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual void OnCompAdding(MElem& aComp);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string());
	virtual TBool OnContentChanged(MElem& aComp, const string& aContName = string());
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
};

/*
 * Agent proxies manager. Owning session to back server
 */
class CobsPxMgr: public MExtIfProv, public MProxyMgr
{
    public:
	CobsPxMgr();
	virtual ~CobsPxMgr();
	void Connect(const string& aSrvUri);
	MProxy* GetProxy(const string& aContext) const;
	// From MProxyMgr
	virtual MProxy* CreateProxy(const string& aId, const string& aContext);
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
	virtual string Oid() const;
	virtual void OnProxyDeleting(const MProxy* aProxy);
	// From MExtIfProv
	virtual MIface* GetEIface(const string& aIfaceId, const string& aIfaceType);
	virtual void SetEnv(MEnv* aEnv);
    protected:
	TBool IsCached(const string& aContext) const;
	void RegProxy(MProxy* aProxy);
	void UnregProxy(const MProxy* aProxy);
    protected:
	MEnv* mEnv;
	string mBackSrvUri;
	string mBackSid;
	string mBackUid;
	BaseClient mBsClient;
	bool mIsConnected;
	map<string, MProxy*> mProxies;
};

#endif
