#ifndef _DAAPROXY_h_
#define _DAAPROXY_h_

#include "mproxy.h"
#include "menv.h"
#include "mlog.h"
#include <map>
#include "../client/bclient.h"


/*
 * Remote environment client. Provides set of clients session to same remote environment
 * This allows to avoid interaction deadlock in case of cyclyc requests, ref ds_pa_msra
 */
class RenvClient
{
    public:
	typedef vector<BaseClient*> TClients;
    public:
	RenvClient();
	~RenvClient();
	void SetRmtSID(const string& aSID);
	void Connect(const string& aHostUri);
	void Disconnect();
	bool Request(const string& aRequest, string& aResponse);
	bool Request(const string& aReqId, const string& aReqArgs, string& aResponse);
	bool IsConnected() const;
    protected:
	BaseClient* GetClient();
    protected:
	TClients mClients;
	// Session ID of remote environment
	string mRmtSID;
	string mHostUri;
};



class DaaProxy;

/*
 * Proxy manager
 */
class DaaPxMgr: public MProxyMgr
{
    public:
	typedef map<string, DaaProxy*> TPxs;
    public:
	DaaPxMgr(MEnv* aEnv, MElem* aOwner, RenvClient& aRenvClient);
	virtual ~DaaPxMgr();
    public:
	// From MProxyMgr
	virtual MProxy* CreateProxy(const string& aId, const string& aContext);
	virtual TBool Request(const string& aContext, const string& aReq, string& aResp);
	virtual string Oid() const;
	virtual void OnProxyDeleting(const MProxy* aProxy);
    protected:
	void RegProxy(DaaProxy* aProxy);
	void UnregProxy(const DaaProxy* aProxy);
	TBool IsCached(const string& aContext) const;
	DaaProxy* GetProxy(const string& aContext) const;
	inline MLogRec* Logger() const;
    protected:
	MEnv* mEnv;
	MElem* mOwner;
	TPxs mProxies;
	RenvClient& mRenvClient;
};

inline MLogRec* DaaPxMgr::Logger() const {return mEnv ? mEnv->Logger(): NULL; }


/*
 * Proxy base
 */
class DaaProxy : public MProxy
{
    public:
	DaaProxy(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~DaaProxy();
	// From MProxy
	virtual const string& GetContext() const;
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	virtual string GetUid() const;
    protected:
	inline MProvider* Provider() const;
	inline MLogRec* Logger() const;
	TBool Request(const string& aReq, string& aResp);
	MIface* NewProxyRequest(const string& aCallSpec, const string& aPxType);
	const MIface* NewProxyRequest(const string& aCallSpec, const string& aPxType) const;
    protected:
	MEnv* mEnv;
	MProxyMgr* mMgr;
	string mContext;
};

inline MProvider* DaaProxy::Provider() const {return mEnv ? mEnv->Provider(): NULL; }

inline MLogRec* DaaProxy::Logger() const {return mEnv ? mEnv->Logger(): NULL; }


#endif 
