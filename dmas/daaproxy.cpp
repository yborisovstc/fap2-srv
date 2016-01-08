
#include "daaproxy.h"


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

bool DaaProxy::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Default implementation, not supporting proxy management
    return false;
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

DaaProxy* DaaProxy::GetProxy(const string& aContext)
{
    __ASSERT(IsCached(aContext));
    return mProxies.at(aContext);
}
