
#include "daaproxy.h"
#include "mipxprov.h"
#include <elem.h>


DaaProxy::DaaProxy(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): mEnv(aEnv), mMgr(aMgr), mContext(aContext)
{
}

DaaProxy::~DaaProxy()
{
}

const string& DaaProxy::GetContext() const
{
    return mContext;
}

TBool DaaProxy::IsCached(const string& aContext) const
{
    return mProxies.count(aContext) > 0;
}

void DaaProxy::RegProxy(DaaProxy* aProxy)
{
    __ASSERT(!IsCached(aProxy->GetContext()));
    mProxies.insert(pair<string, DaaProxy*>(aProxy->GetContext(), aProxy));
}

DaaProxy* DaaProxy::GetProxy(const string& aContext) const
{
    __ASSERT(IsCached(aContext));
    return mProxies.at(aContext);
}

string DaaProxy::Uid() const
{
    return GetContext();
}

string DaaProxy::Mid() const
{
    return string();
}
	
DaaProxy* DaaProxy::CreateProxy(const string& aId, MProxyMgr* aMgr, const string& aContext) const
{
    DaaProxy* res = NULL;
    MElem* eprov = mEnv->Root()->GetNode("ADaaPxProv");
    __ASSERT(eprov != NULL);
    MIpxProv* prov = eprov->GetObj(prov);
    __ASSERT(prov != NULL);
    res = prov->CreateProxy(aId, aMgr, aContext);
    return res;
}

void *DaaProxy::GetIface(const string& aName)
{
    return NULL;
}

const void *DaaProxy::GetIface(const string& aName) const
{
    return NULL;
}

void* DaaProxy::NewProxyRequest(const string& aCallSpec, const string& aPxType)
{ 
    void* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	DaaProxy* px = NULL;
	if (!IsCached(resp)) {
	    px = CreateProxy(aPxType, this, resp);
	    RegProxy(px);
	} else {
	    px = GetProxy(resp);
	}
	res = px->GetIface(aPxType);
    }
    __ASSERT(res != NULL);
    return res;
}

const void* DaaProxy::NewProxyRequest(const string& aCallSpec, const string& aPxType) const
{ 
    const void* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	DaaProxy* self = (DaaProxy*) this;
	DaaProxy* px = NULL;
	if (!IsCached(resp)) {
	    px = CreateProxy(aPxType, self, resp);
	    self->RegProxy(px);
	} else {
	    px = GetProxy(resp);
	}
	res = px->GetIface(aPxType);
    }
    __ASSERT(res != NULL);
    return res;
}

bool DaaProxy::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Just redirect to mgr
   return mMgr->Request(aContext, aReq, aResp);
}
