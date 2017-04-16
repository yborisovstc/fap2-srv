
#include "../server/requests.h"
#include "daaproxy.h"
#include "mipxprov.h"
#include <elem.h>


RenvClient::RenvClient()
{
    // Create primary client session
    BaseClient* client = new BaseClient();
    mClients.push_back(client);
}

RenvClient::~RenvClient()
{
    Disconnect();
}

void RenvClient::Connect(const string& aHostUri)
{
    // Connect primary client
    BaseClient* client = mClients.at(0);
    assert(client != NULL);
    if (client != NULL) {
	client->Connect(aHostUri);
	mHostUri = aHostUri;
    }
}

void RenvClient::Disconnect()
{
    for (TClients::iterator it = mClients.begin(); it != mClients.end(); it++) {
	BaseClient* client = *it;
	client->Disconnect();
	delete client;
    }
    mClients.clear();
}

bool RenvClient::IsConnected() const
{
    bool res = false;
    if (mClients.size() > 0) {
	BaseClient* client = mClients.at(0);
	if (client != NULL) {
	    res = client->IsConnected();
	}
    }
    return res;
}

// TODO [YB] There is still the threads unsafety: it is possible that
// one thread gets the client and starts requesting it (but client status
// is still ready) whereas another thread gets same client - assert will
// happen in BaseClient::Request. To consider safe solution
bool RenvClient::Request(const string& aRequest, string& aResponse)
{
    bool res = false;
    BaseClient* client = GetClient();
    if (client != NULL) {
	//cout << "Client [" << client << "] sent: " << aRequest << endl;
	res = client->Request(aRequest, aResponse);
	//cout << "Client [" << client << "] received: " << aResponse << endl;
    }
    return res;
}

bool RenvClient::Request(const string& aReqId, const string& aReqArgs, string& aResponse)
{
    string req(aReqId);
    req.append(RequestIPC::REQ_SEPARATOR);
    req.append(aReqArgs);
    return Request(req, aResponse);
}

void RenvClient::SetRmtSID(const string& aSID)
{
    assert(mRmtSID.empty());
    mRmtSID = aSID;
}

BaseClient* RenvClient::GetClient()
{
    BaseClient* res = NULL;
    // Search client ready for request
    for (TClients::iterator it = mClients.begin(); it != mClients.end() && res == NULL; it++) {
	BaseClient* client = *it;
	if (client->IsReady()) {
	    res = client;
	}
    }
    // If no one client hasn't found then create new one
    if (res == NULL) {
	BaseClient* client = new BaseClient();
	assert(client != NULL);
	client->Connect(mHostUri);
	assert(!mRmtSID.empty());
	string resp;
	TBool rr = client->Request("EnvProvider", "AttachEnv,1," + mRmtSID, resp);
	if (rr) {
	    res = client;
	    mClients.push_back(client);
	} else {
	    delete client;
	}
    }
    return res;
}





DaaPxMgr::DaaPxMgr(MEnv* aEnv, MElem* aOwner, RenvClient& aRenvClient):
    mEnv(aEnv), mOwner(aOwner), mRenvClient(aRenvClient)
{
}

DaaPxMgr::~DaaPxMgr()
{
    for (TPxs::iterator it = mProxies.begin(); it != mProxies.end(); it++) {
	DaaProxy* px = it->second;
	delete px;
    }
    mProxies.clear();
}

MProxy* DaaPxMgr::CreateProxy(const string& aId, const string& aContext)
{
    DaaProxy* res = NULL;
    // Checking if UID is already proxied
    if (IsCached(aContext)) {
	res = GetProxy(aContext);
    } else {
	MElem* eprov = mEnv->Root()->GetNode("ADaaPxProv");
	__ASSERT(eprov != NULL);
	MIpxProv* prov = eprov->GetObj(prov);
	__ASSERT(prov != NULL);
	res = prov->CreateProxy(aId, this, aContext);
	if (res != NULL) {
	    RegProxy(res);
	} else {
	    Logger()->Write(EErr, mOwner, "Cannot create proxy [%s], context [%s]", aId.c_str(), aContext.c_str());
	}
    }
    return res;
}

void DaaPxMgr::RegProxy(DaaProxy* aProxy)
{
    __ASSERT(aProxy != NULL && !IsCached(aProxy->GetContext()));
    mProxies.insert(pair<string, DaaProxy*>(aProxy->GetContext(), aProxy));
}

void DaaPxMgr::UnregProxy(const DaaProxy* aProxy)
{
    TBool found = EFalse;
    for (TPxs::iterator it = mProxies.begin(); it != mProxies.end(); it++) {
	if (it->second == aProxy) {
	    mProxies.erase(it);
	    found = ETrue;
	    break;
	}
    }
    //__ASSERT(found); It is possible that proxy isn't owned by manager
}

TBool DaaPxMgr::IsCached(const string& aContext) const
{
    return mProxies.count(aContext) > 0;
}

DaaProxy* DaaPxMgr::GetProxy(const string& aContext) const
{
    __ASSERT(IsCached(aContext));
    return mProxies.at(aContext);
}

TBool DaaPxMgr::Request(const string& aContext, const string& aReq, string& aResp)
{
    TBool res =  mRenvClient.Request(aContext, aReq, aResp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s", aContext.c_str(),
		aReq.c_str(), aResp.c_str());
    }
    return res;
}

string DaaPxMgr::Oid() const
{
    // Generating id as relative UID from local root, ref ds_daa_pxdup_birc
    return mOwner->GetUri(mEnv->Root(), ETrue);
}

void DaaPxMgr::OnProxyDeleting(const MProxy* aProxy)
{
    // Unregister proxy
    const DaaProxy* px = dynamic_cast<const DaaProxy*>(aProxy);
    UnregProxy(px);
}



DaaProxy::DaaProxy(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): mEnv(aEnv), mMgr(aMgr), mContext(aContext)
{
}

DaaProxy::~DaaProxy()
{
    mMgr->OnProxyDeleting(this);
}

const string& DaaProxy::GetContext() const
{
    return mContext;
}

MIface* DaaProxy::GetIface(const string& aName)
{
    return NULL;
}

const MIface* DaaProxy::GetIface(const string& aName) const
{
    return NULL;
}

string DaaProxy::GetUid() const
{
    return string();
}

MIface* DaaProxy::NewProxyRequest(const string& aCallSpec, const string& aPxType)
{ 
    MIface* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	if (resp != RequestIPC::RES_OK_NONE) {
	    // Checking if UID is of local Iface to avoid px duplication, ref ds_daa_pxdup
	    TBool isloc = EFalse;
	    string oid, iid;
	    Ifu::ParseUid(resp, oid, iid);
	    if (!Ifu::IsSimpleIid(iid)) {
		MIface* ifc = mEnv->IfaceResolver()->GetIfaceByUid(iid);
		if (ifc != NULL) {
		    res = ifc;
		    isloc = ETrue;
		}
	    }
	    if (!isloc) {
		MProxy* px = mMgr->CreateProxy(aPxType, resp);
		res = px->GetIface(aPxType);
	    }
	}
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s", mContext.c_str(), aCallSpec.c_str(), resp.c_str());
    }
    return res;
}

const MIface* DaaProxy::NewProxyRequest(const string& aCallSpec, const string& aPxType) const
{ 
    const MIface* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	if (resp != RequestIPC::RES_OK_NONE) {
	    // Checking if UID is of local Iface to avoid px duplication, ref ds_daa_pxdup
	    TBool isloc = EFalse;
	    string oid, iid;
	    Ifu::ParseUid(resp, oid, iid);
	    if (!Ifu::IsSimpleIid(iid)) {
		MIface* ifc = mEnv->IfaceResolver()->GetIfaceByUid(iid);
		if (ifc != NULL) {
		    res = ifc;
		    isloc = ETrue;
		}
	    }
	    if (!isloc) {
		MProxy* px = mMgr->CreateProxy(aPxType, resp);
		res = px->GetIface(aPxType);
	    }
	}
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s", mContext.c_str(), aCallSpec.c_str(), resp.c_str());
    }
    return res;
}


MIface* DaaProxy::GetProxy(const string& aSpec, const string& aPxType) const
{ 
    MIface* res = NULL;
    if (aSpec != RequestIPC::RES_OK_NONE) {
	// Checking if UID is of local Iface to avoid px duplication, ref ds_daa_pxdup
	TBool isloc = EFalse;
	string oid, iid;
	Ifu::ParseUid(aSpec, oid, iid);
	if (!Ifu::IsSimpleIid(iid)) {
	    MIface* ifc = mEnv->IfaceResolver()->GetIfaceByUid(iid);
	    if (ifc != NULL) {
		res = ifc;
		isloc = ETrue;
	    }
	}
	if (!isloc) {
	    MProxy* px = mMgr->CreateProxy(aPxType, aSpec);
	    res = px->GetIface(aPxType);
	}
    }
    return res;
}

TBool DaaProxy::Request(const string& aReq, string& aResp)
{
    TBool res = mMgr->Request(mContext, aReq, aResp);
    return res;
}

MIface* DaaProxy::RpcPxN(const string& aName, const string& aIfType) const
{
    string resp;
    TBool rres = mMgr->Request(mContext, Ifu::PackMethod(aName), resp);
    return (rres ? GetProxy(resp, aIfType) : NULL);
}
