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

void MCobsPx::OnCompDeleting(MElem& aComp, TBool aSoft)
{
}

void MCobsPx::OnCompAdding(MElem& aComp)
{
    string resp;
    string uri = aComp.GetUri(NULL, ETrue);
    TBool rr = mMgr->Request(mContext, "OnCompAdding,1," + uri, resp);
    if (!rr) {
	Logger()->Write(MLogRec::EErr, NULL, "OnCompAdding notification of AgentObserver [%s] failed: %s", GetContext().c_str(), resp.c_str());
    }
}

TBool MCobsPx::OnCompChanged(MElem& aComp, const string& aContName)
{
    string resp;
    string uri = aComp.GetUri(NULL, ETrue);
    TBool rr = mMgr->Request(mContext, "OnCompChanged,1," + uri, resp);
    if (!rr) {
	cout << "Error requesting AgentObserver [" << GetContext() << "] OnCompChanged";
    }
    return true;
}

TBool MCobsPx::OnContentChanged(MElem& aComp, const string& aContName)
{
    return true;
}

TBool MCobsPx::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    return true;
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

MIface* CobsPxMgr::GetEIface(const string& aIfaceId, const string& aIfaceType)
{
    MIface* res = NULL;
    __ASSERT(mIsConnected || (mEnv != NULL));
    // Deferred connection
    GUri uri(aIfaceId);
    if (!mIsConnected) {
	if (uri.IsErr()) {
	    mEnv->Logger()->Write(MLogRec::EErr, NULL, "Incorrect ExtIface Uri: %s", aIfaceId.c_str());
	} else {
	    Connect(uri.Scheme() + "://" + uri.Authority());
	}
    }
    MProxy* px = NULL;
    if (!IsCached(aIfaceId)) {
	px = CreateProxy(aIfaceType, uri.Path());
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
	mEnv->Logger()->Write(MLogRec::EErr, NULL, "CobsPxMgr: Connecting to primary environment failed");
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