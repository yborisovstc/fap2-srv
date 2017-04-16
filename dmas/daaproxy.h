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
	MIface* GetProxy(const string& aSpec, const string& aPxType) const;
	const MIface* NewProxyRequest(const string& aCallSpec, const string& aPxType) const;
	template<typename TRet> TRet Rpc(const string& aName) const;
	template<typename TRet, typename TArg1> TRet Rpc(const string& aName, TArg1 aArg) const;
	template<typename TRet, typename TArg1, typename TArg2> TRet Rpc(const string& aName, TArg1 aArg1, TArg2 aArg2) const;
	template<typename TRet, typename TArg1, typename TArg2, typename TArg3> TRet Rpc(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const;
	template<typename TRet> TRet* RpcPx(const string& aName) const;
	template<typename TRet> const TRet* RpcPxC(const string& aName) const;
	template<typename TRet, typename TArg1> TRet* RpcPx(const string& aName, TArg1 aArg) const;
	template<typename TRet, typename TArg1> const TRet* RpcPxC(const string& aName, TArg1 aArg) const;
	template<typename TRet, typename TArg1, typename TArg2> TRet* RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2) const;
	template<typename TRet, typename TArg1, typename TArg2, typename TArg3> TRet* RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const;
	template<typename TRet, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	    TRet* RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3, TArg4 aArg4) const;
	template<typename TArg1> void Rpcv(const string& aName, TArg1 aArg) const;
	template<typename TArg1, typename TArg2> void Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2) const;
	template<typename TArg1, typename TArg2, typename TArg3> void Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const;
	template<typename TArg1, typename TArg2, typename TArg3, typename TArg4> void Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3, TArg4 aArg4) const;
	MIface* RpcPxN(const string& aName, const string& aIfType) const;
	template<typename TArg1> MIface* RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1) const;
	template<typename TArg1, typename TArg2> MIface* RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1, TArg2 aArg2) const;
	template<typename TArg1, typename TArg2, typename TArg3> MIface* RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const;
    protected:
	MEnv* mEnv;
	MProxyMgr* mMgr;
	string mContext;
};

inline MProvider* DaaProxy::Provider() const {return mEnv ? mEnv->Provider(): NULL; }

inline MLogRec* DaaProxy::Logger() const {return mEnv ? mEnv->Logger(): NULL; }

template<typename TRet> TRet DaaProxy::Rpc(const string& aName) const
{
    string resp;
    TRet res;
    TBool rr = mMgr->Request(mContext, Ifu::PackMethod(aName), resp);
    if (rr) res = Ifu::Unpack(resp, res);
    return res;
}

template<typename TRet, typename TArg1> TRet DaaProxy::Rpc(const string& aName, TArg1 aArg) const
{
    string resp;
    TRet res;
    TBool rr = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg), resp);
    if (rr) res = Ifu::Unpack(resp, res);
    return res;
}

template<typename TRet, typename TArg1, typename TArg2> TRet DaaProxy::Rpc(const string& aName, TArg1 aArg1, TArg2 aArg2) const
{
    string resp;
    TRet res;
    TBool rr = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2), resp);
    if (rr) res = Ifu::Unpack(resp, res);
    return res;
}

template<typename TRet, typename TArg1, typename TArg2, typename TArg3> TRet DaaProxy::Rpc(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const
{
    string resp;
    TRet res;
    TBool rr = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3), resp);
    if (rr) res = Ifu::Unpack(resp, res);
    return res;
}


template<typename TRet> TRet* DaaProxy::RpcPx(const string& aName) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName), resp);
    return (rres ? (TRet*) GetProxy(resp, TRet::Type()) : NULL);
} 

template<typename TRet> const TRet* DaaProxy::RpcPxC(const string& aName) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName), resp);
    return (rres ? (const TRet*) GetProxy(resp, TRet::Type()) : NULL);
} 

template<typename TRet, typename TArg1> TRet* DaaProxy::RpcPx(const string& aName, TArg1 aArg) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg), resp);
    return (rres ? (TRet*) GetProxy(resp, TRet::Type()) : NULL);
}

template<typename TRet, typename TArg1> const TRet* DaaProxy::RpcPxC(const string& aName, TArg1 aArg) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg), resp);
    return (rres ? (const TRet*) GetProxy(resp, TRet::Type()) : NULL);
}

template<typename TRet, typename TArg1, typename TArg2> TRet* DaaProxy::RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2), resp);
    return (rres ? (TRet*) GetProxy(resp, TRet::Type()) : NULL);
}

template<typename TRet, typename TArg1, typename TArg2, typename TArg3> TRet* DaaProxy::RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3), resp);
    return (rres ? (TRet*) GetProxy(resp, TRet::Type()) : NULL);
}

template<typename TRet, typename TArg1, typename TArg2, typename TArg3, typename TArg4> TRet* DaaProxy::RpcPx(const string& aName, TArg1 aArg1, TArg2 aArg2,
	TArg3 aArg3, TArg4 aArg4) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3, aArg4), resp);
    return (rres ? (TRet*) GetProxy(resp, TRet::Type()) : NULL);
}


template<typename TArg1> void DaaProxy::Rpcv(const string& aName, TArg1 aArg) const
{
    string resp;
    mMgr->Request(mContext, Ifu::PackMethod(aName, aArg), resp);
}

template<typename TArg1, typename TArg2> void DaaProxy::Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2) const
{
    string resp;
    mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2), resp);
}

template<typename TArg1, typename TArg2, typename TArg3> void DaaProxy::Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const
{
    string resp;
    mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3), resp);
}

template<typename TArg1, typename TArg2, typename TArg3, typename TArg4> void DaaProxy::Rpcv(const string& aName, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3, TArg4 aArg4) const
{
    string resp;
    mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3, aArg4), resp);
}
 
template<typename TArg1> MIface* DaaProxy::RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1), resp);
    return (rres ? GetProxy(resp, aIfType) : NULL);
}
 
template<typename TArg1, typename TArg2> MIface* DaaProxy::RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1, TArg2 aArg2) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2), resp);
    return (rres ? GetProxy(resp, aIfType) : NULL);
}
 
template<typename TArg1, typename TArg2, typename TArg3> MIface* DaaProxy::RpcPxN(const string& aName, const string& aIfType, TArg1 aArg1, TArg2 aArg2, TArg3 aArg3) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName, aArg1, aArg2, aArg3), resp);
    return (rres ? GetProxy(resp, aIfType) : NULL);
}


#endif 
