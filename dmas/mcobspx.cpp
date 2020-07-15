#include "mcobspx.h"
#include <stdexcept> 
#include <mlog.h>
#include "../server/requests.h"

EIfacePx::EIfacePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): mEnv(aEnv), mMgr(aMgr), mContext(aContext)
{
}

EIfacePx::~EIfacePx()
{
}

const string& EIfacePx::GetContext() const
{
    return mContext;
}


MCobsPx::MCobsPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}


MCobsPx::~MCobsPx()
{
}

string MCobsPx::Uid() const
{
    return GetContext();
}

string MCobsPx::Mid() const
{
    return string();
}

MIface* MCobsPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MAgentObserver::Type()) {
	res = (MAgentObserver*) this;
    }
    return res;
}

const MIface* MCobsPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MAgentObserver::Type()) {
	res = (const MAgentObserver*) this;
    }
    return res;
}
	
MIface* MCobsPx::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	    throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	    throw (runtime_error("Wrong arguments number"));
    return res;
}

void MCobsPx::OnCompDeleting(const MUnit* aComp, TBool aSoft, TBool aModif)
{
    return Rpcv<const MUnit*, TBool, TBool>(__func__, aComp, aSoft, aModif);
}

void MCobsPx::OnCompAdding(const MUnit* aComp, TBool aModif)
{
    return Rpcv<const MUnit*, TBool>(__func__, aComp, aModif);
}

TBool MCobsPx::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    return Rpc<TBool, const MUnit*, const string&, TBool>(__func__, aComp, aContName, aModif);
}

TBool MCobsPx::OnChanged(const MUnit* aComp)
{
    return Rpc<TBool, const MUnit*>(__func__, aComp);
}

TBool MCobsPx::OnCompRenamed(const MUnit* aComp, const string& aOldName)
{
    return Rpc<TBool, const MUnit*, const string&>(__func__, aComp, aOldName);
}

void MCobsPx::OnCompMutated(const MUnit* aNode)
{
    return Rpcv<const MUnit*>(__func__, aNode);
}


CobsPxMgr::CobsPxMgr(): mEnv(NULL), mIsConnected(false)
{
}

CobsPxMgr::~CobsPxMgr()
{
    if (mIsConnected) {
	mBsClient.Disconnect();
    }
}

bool CobsPxMgr::Request(const string& aContext, const string& aReq, string& aResp)
{
    string req(aContext);
    req.append(RequestIPC::REQ_SEPARATOR);
    req.append(aReq);
    return mBsClient.Request(req, aResp);
}

// TODO YB What if there is request from another NW node then CobsPxMgr is connected to?
MIface* CobsPxMgr::GetEIface(const string& aIfaceId, const string& aIfaceType)
{
    MIface* res = NULL;
    __ASSERT(mIsConnected || (mEnv != NULL));
    // Deferred connection
    GUri uri(aIfaceId);
    if (!mIsConnected) {
	if (uri.IsErr()) {
	    mEnv->Logger()->Write(EErr, NULL, "Incorrect ExtIface Uri: %s", aIfaceId.c_str());
	} else {
	    Connect(uri.Scheme() + "://" + uri.Authority());
	}
    }
    MProxy* px = NULL;
    if (!IsCached(aIfaceId)) {
	px = CreateProxy(aIfaceType, aIfaceId);
	RegProxy(px);
    } else {
	px = GetProxy(aIfaceId);
    }
    res = px->GetIface(aIfaceType);
    return res;
}

void CobsPxMgr::SetEnv(MEnv* aEnv)
{
    __ASSERT(mEnv == NULL);
    mEnv = aEnv;
}

void CobsPxMgr::Connect(const string& aSrvUri)
{
    try {
	mBsClient.Connect(aSrvUri);
	mIsConnected = true;
    } catch (exception& e) {
	mEnv->Logger()->Write(EErr, NULL, "CobsPxMgr: Connecting to primary environment failed");
    }
}


MProxy* CobsPxMgr::CreateProxy(const string& aId, const string& aContext)
{
    MProxy* res = NULL;
    if (aId == MAgentObserver::Type()) {
	res = new MCobsPx(mEnv, this, aContext);
    } else {
	__ASSERT(EFalse);
    }
    return res;
}

TBool CobsPxMgr::IsCached(const string& aContext) const
{
    return mProxies.count(aContext) > 0;
}

MProxy* CobsPxMgr::GetProxy(const string& aContext) const
{
    __ASSERT(IsCached(aContext));
    return mProxies.at(aContext);
}

void CobsPxMgr::RegProxy(MProxy* aProxy)
{
    __ASSERT(!IsCached(aProxy->GetContext()));
    mProxies.insert(pair<string, MProxy*>(aProxy->GetContext(), aProxy));
}

string CobsPxMgr::Oid() const
{
    return string();
}

void CobsPxMgr::OnProxyDeleting(const MProxy* aProxy)
{
    // Unregister proxy
    UnregProxy(aProxy);
}

void CobsPxMgr::UnregProxy(const MProxy* aProxy)
{
    TBool found = EFalse;
    for (map<string, MProxy*>::iterator it = mProxies.begin(); it != mProxies.end(); it++) {
	if (it->second == aProxy) {
	    mProxies.erase(it);
	    found = ETrue;
	    break;
	}
    }
    __ASSERT(found);
}
