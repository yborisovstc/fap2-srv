
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
	    Logger()->Write(MLogRec::EErr, mOwner, "Cannot create proxy [%s], context [%s]", aId.c_str(), aContext.c_str());
	}
    }
    return res;
}

void DaaPxMgr::RegProxy(DaaProxy* aProxy)
{
    __ASSERT(aProxy != NULL && !IsCached(aProxy->GetContext()));
    mProxies.insert(pair<string, DaaProxy*>(aProxy->GetContext(), aProxy));
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
    return mRenvClient.Request(aContext, aReq, aResp);
}

string DaaPxMgr::Oid() const
{
    return mOwner->GetUri(mEnv->Root()->GetMan(), ETrue);
}




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

MIface* DaaProxy::GetIface(const string& aName)
{
    return NULL;
}

const MIface* DaaProxy::GetIface(const string& aName) const
{
    return NULL;
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
	Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: request [%s] failed: %s", mContext.c_str(), aCallSpec.c_str(), resp.c_str());
    }
    return res;
}

const MIface* DaaProxy::NewProxyRequest(const string& aCallSpec, const string& aPxType) const
{ 
    const MIface* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres && resp != RequestIPC::RES_OK_NONE) {
	MProxy* px = mMgr->CreateProxy(aPxType, resp);
	res = px->GetIface(aPxType);
    }
    return res;
}

