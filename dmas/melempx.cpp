
#include <mprov.h>
#include <elem.h>
#include "melempx.h"
#include "mvertpx.h"
#include "mipxprov.h"
#include <stdexcept> 
#include "../server/requests.h"

const string FakeString = "";
vector<MElem*> FakeComps;

MelemPx::IfIter::IfIter(MelemPx* aHost, const string& aIfName, const TICacheRCtx& aReq, int aInd): mHost(aHost), mIfName(aIfName), mReq(aReq), mInd(aInd)
{}

MelemPx::IfIter::IfIter(const IfIter& aIt): mHost(aIt.mHost), mIfName(aIt.mIfName), mInd(aIt.mInd) {}

MElem::MIfIter& MelemPx::IfIter::operator=(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    mHost = it.mHost; 
    mInd = it.mInd;
    mIfName = it.mIfName;
    mReq = it.mReq;
    return *this;
}

MElem::MIfIter* MelemPx::IfIter::Clone() const
{
    return new IfIter(*this);
}

MElem::MIfIter& MelemPx::IfIter::operator++()
{
    mInd++;
    return *this;
}

TBool MelemPx::IfIter::operator==(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    TBool res = (mHost == it.mHost && mInd == it.mInd && mIfName == it.mIfName && mReq == it.mReq);
    return res;
}

void*  MelemPx::IfIter::operator*()
{
    void* res = mHost->GetIfind(mIfName, mReq, mInd);
    return res;
}



MelemPx::MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
    mChromo = mEnv->Provider()->CreateChromo();
}

MelemPx::~MelemPx()
{
    delete mChromo;
}

void MelemPx::Delete()
{
    string resp;
    string req("Delete,1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (!rr) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

bool MelemPx::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Just redirect to mgr
   return mMgr->Request(aContext, aReq, aResp);
}

MIface* MelemPx::Call(const string& aSpec, string& aRes)
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
    if (name == "Name") {
	aRes = Name();
    } else if (name == "GetMan") {
	res = GetMan();
    } else if (name == "GetParent") {
	res = GetParent();
    } else if (name == "GetRoot") {
	res = GetRoot();
    } else if (name == "IsProvided") {
	TBool rr = IsProvided();
	aRes = Ifu::FromBool(rr);
    } else if (name == "GetNode") {
	res = GetNode(args.at(0));
    } else if (name == "GetCont") {
	GetCont(aRes, args.at(0));
    } else if (name == "GetUri") {
	aRes = GetUri(NULL, ETrue);
    } else if (name == "GetUri#2") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "DoGetObj") {
	res = (MIface*) DoGetObj(args.at(0).c_str());
    } else if (name == "GetIfi") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "GetIfind") {
	string name = args.at(0);
	res = (MIface*) NewProxyRequest(aSpec, name);
    } else if (name == "GetSIfi") {
	string name = args.at(0);
	res = (MIface*) NewProxyRequest(aSpec, name);
    } else if (name == "UnregIfReq") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "UnregIfProv") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "OnCompAdding") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "CompsCount") {
	mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "GetComp") {
	res = (MIface*) NewMElemProxyRequest(aSpec);
    } else {
	mMgr->Request(mContext, aSpec, aRes);
    }
    return res;
}

MElem* MelemPx::NewMElemProxyRequest(const string& aCallSpec)
{
    return (MElem*) NewProxyRequest(aCallSpec, MElem::Type());
}

const MElem* MelemPx::NewMElemProxyRequest(const string& aCallSpec) const
{
    return (const MElem*) NewProxyRequest(aCallSpec, MElem::Type());
}

const string MelemPx::EType(TBool aShort) const
{
    string resp;
    mMgr->Request(mContext, "EType,1," + Ifu::FromBool(aShort), resp);
    return resp;
}

const string& MelemPx::Name() const
{
    string resp;
    TBool res = mMgr->Request(mContext, "Name", resp);
    if (res) 
	((MelemPx*) this)->mName = resp;
    return mName;
}

MElem* MelemPx::GetMan()
{
    return NewMElemProxyRequest("GetMan");
}

const MElem* MelemPx::GetMan() const
{
    return NewMElemProxyRequest("GetMan");
}

void MelemPx::GetCRoot(TMut& aMut) const
{
    string resp;
    string req("GetCRoot,1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	aMut = resp;
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

// Ignoring. Only one way relation is allowed for the proxy
void MelemPx::SetMan(MElem* aMan)
{
}

void MelemPx::SetObserver(MAgentObserver* aObserver)
{
    __ASSERT(false);
}

vector<MElem*>& MelemPx::Comps()
{ 
    return FakeComps;
}

const vector<MElem*>& MelemPx::Comps() const { return FakeComps;}

MElem* MelemPx::GetNode(const string& aUri, TBool aInclRm) 
{ 
    MElem* res = NULL;
    string cspec = "GetNode,1," + aUri;
    res = NewMElemProxyRequest(cspec);
    return res;
}

MElem* MelemPx::GetNode(const GUri& aUri, TBool aInclRm)
{
    return GetNode(aUri.GetUri(ETrue), aInclRm);
}

MElem* MelemPx::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm)
{ 
    MElem* res = NULL;
    string suri = aUri.GetUri(aPathBase);
    string cspec = "GetNode#2,1," + suri + "," + Ifu::FromBool(aAnywhere) + "," + Ifu::FromBool(aInclRm);
    res = NewMElemProxyRequest(cspec);
    return res;
}

MElem* MelemPx::GetRoot() const
{
    return ((MelemPx*) this)->NewMElemProxyRequest("GetRoot");
}

MElem* MelemPx::GetInhRoot() const
{
    __ASSERT(false);
    return NULL;
}

TInt MelemPx::GetContCount(const string& aName) const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "GetContCount,1," + aName, resp);
    if (rr) {
	res = Ifu::ToInt(resp);
    }
    return res;
}

TBool MelemPx::ContentExists(const string& aName) const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "ContentExists,1," + aName, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

// TODO [YB] Needs to return call status
TBool MelemPx::GetCont(string& aCont, const string& aName) const
{
    __ASSERT(EFalse); // To implement
    mMgr->Request(mContext, "GetCont,1," + aName, aCont);
    return EFalse;
}
 
string MelemPx::GetContComp(const string& aOwnerName, TInt aInd) const
{
    string resp;
    string req = Ifu::CombineIcSpec("GetContComp", "1");
    Ifu::AddIcSpecArg(req, aOwnerName);
    Ifu::AddIcSpecArg(req, Ifu::FromInt(aInd));
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
	return string();
    } else {
	return resp;
    }
}

string MelemPx::GetContent(const string& aName, TBool aFull) const
{
    string resp;
    string req = Ifu::CombineIcSpec("GetContent", "1");
    Ifu::AddIcSpecArg(req, aName);
    Ifu::AddIcSpecArg(req, Ifu::FromBool(aFull));
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
	return string();
    } else {
	return resp;
    }
}

TBool MelemPx::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) {return false;}
 
TBool MelemPx::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode) {return false;}

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    string ctx = aCtx->GetUri(NULL, ETrue);
    string req = Ifu::CombineIcSpec("Mutate#2", "1");
    Ifu::AddIcSpecArg(req, aRunTimeOnly);
    Ifu::AddIcSpecArg(req, aCheckSafety);
    Ifu::AddIcSpecArg(req, aTrialMode);
    Ifu::AddIcSpecArg(req, ctx);
    string resp;
    TBool rres = mMgr->Request(mContext, req, resp);
    if (!rres) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [Mutate#2] failed: %s", Uid().c_str(), resp.c_str());
    }
}

void MelemPx::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
}

void MelemPx::GetUri(GUri& aUri, MElem* aTop) const
{
    string uri = GetUri(aTop, ETrue);
    GUri guri(uri);
    aUri.Prepend(guri);
}

void MelemPx::GetRUri(GUri& aUri, MElem* aTop)
{
    string res = GetRUri(aTop);
    aUri = GUri(res);
}

string MelemPx::GetUri(MElem* aTop, TBool aShort) const
{
    string res;
    if (aTop == NULL) {
	TBool rres = mMgr->Request(mContext, "GetUri,1", res);
	if (!rres) {
	    Logger()->Write(EErr, NULL, "Proxy [%s]: [GetUri] request failed: %s",
		    Uid().c_str(), res.c_str());
	}
    } else {
	string base;
	/*
	TBool rres = mMgr->Request(mContext, "GetUri,1", base);
	if (!rres) {
	    Logger()->Write(EErr, NULL, "Proxy [%s]: [GetUri] failed on getting base: %s",
		    Uid().c_str(), base.c_str());
	}
	*/
	base = aTop->GetUri();
	TBool rres = mMgr->Request(mContext, "GetUri#2,1," + base, res);
	if (!rres) {
	    Logger()->Write(EErr, NULL, "Proxy [%s]: [GetUri %s] failed on GetUri#2 request: %s",
		    Uid().c_str(), base.c_str(), res.c_str());
	}
    }
    return res;
}

string MelemPx::GetRUri(MElem* aTop)
{
    string base = aTop->GetUri();
    string res;
    TBool rres = mMgr->Request(mContext, "GetRUri,1," + base, res);
    if (!rres) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [GetRUri %s] request failed: %s",
		Uid().c_str(), base.c_str(), res.c_str());
    }
    return res;
}

TBool MelemPx::RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes) {return false;}

TBool MelemPx::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes) {return false;}

MElem* MelemPx::GetAowner()
{
    return NewMElemProxyRequest("GetAowner,1");
}

const MElem* MelemPx::GetAowner() const
{
    return NewMElemProxyRequest("GetAowner,1");
}

MElem* MelemPx::GetAcompOwning(MElem* aComp)
{
    __ASSERT(false);
    return NULL;
}

MElem* MelemPx::GetUpperAowner()
{
    __ASSERT(false);
    return NULL;
}

MElem* MelemPx::GetCommonOwner(MElem* aElem)
{
    __ASSERT(false);
    return NULL;
}

TBool MelemPx::IsRemoved() const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "IsRemoved", resp);
    if (rr) 
	res = Ifu::ToBool(resp);
    return res;
}

void MelemPx::SetRemoved(TBool aModif)
{
    __ASSERT(false);
}

MElem* MelemPx::GetAttachedMgr()
{
    __ASSERT(false);
    return NULL;
}

const MElem* MelemPx::GetAttachedMgr() const
{
    __ASSERT(false);
    return NULL;
}

TBool MelemPx::IsAownerOf(const MElem* aElem) const
{
    __ASSERT(false);
    return false;
}

TBool MelemPx::ChangeAttr(TNodeAttr aAttr, const string& aVal)
{
    __ASSERT(false);
    return false;
}

void MelemPx::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
}

void MelemPx::GetRank(Rank& aRank) const
{
    __ASSERT(false);
};

void MelemPx::GetRank(Rank& aRank, const ChromoNode& aMut) const
{
    __ASSERT(false);
}

void MelemPx::GetCompRank(Rank& aRank, const MElem* aComp) const
{
    __ASSERT(false);
};

TInt MelemPx::GetCompLrank(const MElem* aComp) const
{
    TInt res = -1;
    string resp;
    string req = Ifu::CombineIcSpec("GetCompLrank", "1");
    string uri = aComp->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToInt(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: Request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
    return res;
}
 
MElem* MelemPx::GetCompOwning(const string& aParent, MElem* aElem)
{
    __ASSERT(false);
    return NULL;
}

MElem* MelemPx::GetCompOwning(MElem* aElem)
{
    string req = Ifu::CombineIcSpec("GetCompOwning", "1");
    string uri = aElem->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    MElem* res = NewMElemProxyRequest(req);
    return res;
}

const MElem* MelemPx::GetCompOwning(const MElem* aElem) const
{
    string req = Ifu::CombineIcSpec("GetCompOwning", "1");
    string uri = aElem->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    return NewMElemProxyRequest(req);
}

TBool MelemPx::IsInheritedComp(const MElem* aNode) const
{
    __ASSERT(false);
    return false;
}

MElem* MelemPx::GetCompAowner(const MElem* aComp)
{
    string req = Ifu::CombineIcSpec("GetCompAowner", "1");
    string uri = aComp->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    return NewMElemProxyRequest(req);
}

const MElem* MelemPx::GetCompAowner(const MElem* aComp) const
{
    string req = Ifu::CombineIcSpec("GetCompAowner", "1");
    string uri = aComp->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    return NewMElemProxyRequest(req);
}

TBool MelemPx::HasInherDeps(const MElem* aScope) const
{
    __ASSERT(false);
    return false;
}

// TODO [YB] To implement
TInt MelemPx::GetCapacity() const
{
    return 0;
}

TBool MelemPx::IsHeirOf(const string& aParent) const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "IsHeirOf,1," + aParent, resp);
    if (rr) 
	res = Ifu::ToBool(resp);
    return res;
}

TInt MelemPx::CompsCount() const
{
    TInt res = 0;
    string resp;
    TBool rr = mMgr->Request(mContext, "CompsCount", resp);
    if (rr)
	res = Ifu::ToInt(resp);
    return res;
}

/*
MElem* MelemPx::GetComp(const string& aParent, const string& aName)
{
    __ASSERT(false);
    return NULL;
};
*/

MElem* MelemPx::GetComp(const string& aParent, const string& aName) const
{
    string req = Ifu::CombineIcSpec("GetComp#2", "1");
    Ifu::AddIcSpecArg(req, aParent);
    Ifu::AddIcSpecArg(req, aName);
    return ((MelemPx*) this)->NewMElemProxyRequest(req);
}

TBool MelemPx::IsCompAttached(const MElem* aComp) const
{
    TBool res = EFalse;
    string resp;
    string req("IsCompAttached,1,");
    req += aComp->GetUri(NULL, ETrue);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
    return res;
}

void MelemPx::DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
}

TBool  MelemPx::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
    return false;
}

MElem*  MelemPx::CreateHeir(const string& aName, MElem* aMan, MElem* aContext)
{
    MElem* heir = NULL;
    if (IsProvided()) {
	heir = Provider()->CreateNode(Name(), aName, aMan, mEnv);
	// Persistently attach heir to final owner, but also set context for mutation, ref ds_daa_itn_sfo
	aMan->AppendComp(heir);
	heir->SetCtx(aContext);
	// Register final URI, ref ds_daa_itn_sfu
	//GUri fu(aFinalUri);
	//fu.AppendElem(string(), aName);
	//mEnv->RegisterUri(fu, heir);
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	MElem* hprnt = heir->GetParent();
	hprnt->RemoveChild(heir);
	heir->SetParent(hprnt);
    }
    else {
	MElem* parent = GetParent();
	__ASSERT(parent != NULL);
	MElem* owner = GetMan();
	__ASSERT(owner != NULL);
	if (parent->IsProvided()) {
	    // Parent is Agent - native element. Create via provider
	    heir = Provider()->CreateNode(EType(), aName, aMan, mEnv);
	    // Persistently attach heir to final owner, but also set context for mutation, ref ds_daa_itn_sfo
	    aMan->AppendComp(heir);
	    heir->SetCtx(owner);
	    // Register final URI, ref ds_daa_itn_sfu
	    //GUri fu(aFinalUri);
	    //fu.AppendElem(string(), aName);
	    //mEnv->RegisterUri(fu, heir);
	    // Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	    MElem* hprnt = heir->GetParent();
	    hprnt->RemoveChild(heir);
	    heir->SetParent(hprnt);
	}
	else {
	    heir = parent->CreateHeir(aName, aMan, owner);
	}
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	string cspec = GetChromoSpec();
	Chromo* chr = Provider()->CreateChromo();
	chr->SetFromSpec(cspec);
	ChromoNode& root = chr->Root();
	heir->SetMutation(root);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	heir->Mutate(ETrue, EFalse, EFalse, heir);
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetParent(Name());
	// Relocate heir to hier from which the request of creating heir came
	heir->SetCtx(NULL);
	heir->SetCtx(aContext);
	// Using full comp-owner relation, ref ds_di_cnfr_susl
	//owner->RemoveComp(heir);
	//aMan->AppendComp(heir);
	// Re-adopt the child, obtain real parent of child (it can be local native agent but not parent's proxy), ref. ds_daa_hhb
	// Using "light" one-way relation on creation phase, ref. ds_daa_hunv
	heir->SetParent(NULL);
	heir->SetParent(this);
	delete chr;
    }
    return heir;
}

MElem*  MelemPx::AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
    return NULL;
}

TBool  MelemPx::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    __ASSERT(false);
    return false;
}

const MChromo&  MelemPx::Chromos() const { return *mChromo;}

MChromo&  MelemPx::Chromos() { return *mChromo;}

void  MelemPx::SetMutation(const ChromoNode& aMuta)
{
    __ASSERT(false);
}

TBool  MelemPx::AppendMutation(const string& aFileName)
{
    __ASSERT(false);
    return false;
}

// TODO [YB] This is short-term solutin only. We need to avoid using tree-type mutations
// in base agent APIs. To migrate to pure OSM solution, ref ds_daa_ttm 
ChromoNode  MelemPx::AppendMutation(const ChromoNode& aMuta)
{
    string resp;
    string req = Ifu::CombineIcSpec("AppendMutation#2", "1", aMuta);
    mMgr->Request(mContext, req, resp);
}

ChromoNode MelemPx::AppendMutation(TNodeType aType)
{
    __ASSERT(false);
    return ChromoNode();
}

void MelemPx::AppendMutation(const TMut& aMut)
{
    string resp;
    string req = Ifu::CombineIcSpec("AppendMutation", "1", aMut);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (!rr) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [AppendMutation] request failed: %s",
		Uid().c_str(), resp.c_str());
    }
}

TMDeps&  MelemPx::GetMDeps() { return mMDeps;}

void  MelemPx::AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr)
{
    __ASSERT(false);
}

void  MelemPx::AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode)
{
    __ASSERT(false);
}

void  MelemPx::RemoveMDep(const TMDep& aDep, const MElem* aContext)
{
    __ASSERT(false);
}

TBool  MelemPx::RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext)
{
    __ASSERT(false);
    return false;
}

TBool  MelemPx::IsChromoAttached() const
{
    TBool res = EFalse;
    string resp;
    string req("IsChromoAttached,1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
    return res;
}

unique_ptr<MChromo> MelemPx::GetFullChromo() const
{
    __ASSERT(false);
};

void  MelemPx::GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly, TBool aAnyType) const
{
    __ASSERT(false);
}

TMDep MelemPx::GetMajorDep()
{
    __ASSERT(false);
    return mMDeps.at(0);
}

void MelemPx::GetMajorDep(TMDep& aDep, TBool aUp, TBool aDown)
{
    __ASSERT(false);
}

TMDep MelemPx::GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel)
{
    __ASSERT(false);
    return mMDeps.at(0);
}

void  MelemPx::GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp, TBool aDown)
{
    __ASSERT(false);
}

TBool  MelemPx::CompactChromo()
{
    __ASSERT(false);
    return false;
}

TBool MelemPx::CompactChromo(const ChromoNode& aNode)
{
    __ASSERT(false);
    return false;
}

void MelemPx::OnNodeMutated(const MElem* aNode, const TMut& aMut, const MElem* aCtx)
{
    string resp;
    string req = Ifu::CombineIcSpec("OnNodeMutated", "1");
    string uri = aNode->GetUri(NULL, ETrue);
    string ctx = aCtx != NULL ? aCtx->GetUri(NULL, ETrue) : string("Nothing");
    Ifu::AddIcSpecArg(req, uri);
    Ifu::AddIcSpecArg(req, aMut);
    Ifu::AddIcSpecArg(req, ctx);
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [OnNodeMutated %s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

void MelemPx::OnParentMutated(MElem* aParent, const TMut& aMut)
{
    __ASSERT(false);
};

void  MelemPx::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    string resp;
    string req = Ifu::CombineIcSpec("OnCompDeleting", "1");
    string uri = aComp.GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    Ifu::AddIcSpecArg(req, Ifu::FromBool(aSoft));
    mMgr->Request(mContext, req, resp);
}

void  MelemPx::OnCompAdding(MElem& aComp, TBool aModif)
{
    string resp;
    string req("OnCompAdding,1,");
    string uri = aComp.GetUri(NULL, ETrue);
    req += uri;
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [OnCompAdding %s] request failed: %s",
		Uid().c_str(), uri.c_str(), resp.c_str());
    }
}

TBool  MelemPx::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("OnCompChanged", "1");
    string uri = aComp.GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    Ifu::AddIcSpecArg(req, aContName);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

TBool  MelemPx::OnChanged(MElem& aComp)
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("OnChanged", "1");
    string uri = aComp.GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

TBool MelemPx::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("OnCompRenamed", "1");
    string uri = aComp.GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    Ifu::AddIcSpecArg(req, aOldName);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

TBool  MelemPx::IsComp(const MElem* aElem) const
{
    TBool res = EFalse;
    string resp;
    string req("IsComp,1,");
    req += aElem->GetUri(NULL, ETrue);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
    return res;
}

TBool  MelemPx::AppendComp(MElem* aComp, TBool aRt)
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("AppendComp", "1");
    string uri = aComp->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rres = mMgr->Request(mContext, req, resp);
    if (rres) {
	res = Ifu::ToBool(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
    return res;
}

void MelemPx::RemoveComp(MElem* aComp)
{
    string resp;
    string req = Ifu::CombineIcSpec("RemoveComp", "1");
    string uri = aComp->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rres = mMgr->Request(mContext, req, resp);
    if (!rres) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

// MParent
	
void  MelemPx::OnChildDeleting(MElem* aChild)
{
    string resp;
    string req = Ifu::CombineIcSpec("OnChildDeleting", "1");
    string uri = aChild->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

TBool  MelemPx::OnChildRenamed(MElem* aComp, const string& aOldName)
{
    __ASSERT(false);
    return false;
}

TBool  MelemPx::AppendChild(MElem* aChild)
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("AppendChild", "1");
    string uri = aChild->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rres = mMgr->Request(mContext, req, resp);
    if (rres) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

void  MelemPx::RemoveChild(MElem* aChild)
{
    string resp;
    string req = Ifu::CombineIcSpec("RemoveChild", "1");
    string uri = aChild->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool rres = mMgr->Request(mContext, req, resp);
    if (!rres) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}


// MChild

MElem*  MelemPx::GetParent()
{
    return NewMElemProxyRequest("GetParent");
}

const MElem*  MelemPx::GetParent() const
{
    __ASSERT(false);
    return NULL;
}

void  MelemPx::OnParentDeleting(MElem* aParent)
{
    string resp;
    string req = Ifu::CombineIcSpec("OnParentDeleting", "1");
    string uri = aParent->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

void  MelemPx::SetParent(const string& aParent)
{
    __ASSERT(false);
}

void  MelemPx::SetParent(MElem* aParent)
{
    string resp;
    string req = Ifu::CombineIcSpec("SetParent", "1");
    string uri = aParent->GetUri(NULL, ETrue);
    Ifu::AddIcSpecArg(req, uri);
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}


// MIfProv

void*  MelemPx::GetSIfiC(const string& aName, Base* aRequestor)
{
    RqContext ctx(aRequestor);
    return GetSIfi(aName, &ctx);
}

// TODO [YB] To use ctx operator string() for serialization of ctx
void*  MelemPx::GetSIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    string resp;
    string req("GetSIfi,1,");
    req += aName + Ifu::KRinvSep;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	if (rq == NULL) break;
	MElem* re = rq->GetObj(re);
	string reuri = re->GetUri(NULL, ETrue);
	req += reuri + Ifu::KArraySep;
	cct = cct->Ctx();
    }
    res = NewProxyRequest(req, aName);
    return res;
}

void*  MelemPx::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert)
{
    __ASSERT(false);
    return NULL;
}

MElem::TIfRange  MelemPx::GetIfi(const string& aName, const RqContext* aCtx)
{
    string resp;
    string req("GetIfi,1,");
    req += aName + Ifu::KRinvSep;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	if (rq != NULL) {
	    MElem* re = rq->GetObj(re);
	    string reuri = re->GetUri(NULL, ETrue);
	    req += reuri + Ifu::KArraySep;
	}
	cct = cct->Ctx();
    }
    TBool res = mMgr->Request(mContext, req, resp);
    TInt ifcnt = 0;
    if (res) 
	ifcnt = Ifu::ToInt(resp);
    TICacheRCtx rctx(aCtx);
    IfIter beg(this, aName, rctx, 0);
    IfIter end(this, aName, rctx, ifcnt);
    return TIfRange(beg, end);
}

// Debugging

void MelemPx::DumpMcDeps() const
{
};

void MelemPx::DumpCmDeps() const
{};

MElem* MelemPx::GetNodeS(const char* aUri)
{
    __ASSERT(false);
};

MElem* MelemPx::GetComp(TInt aInd)
{
    return NewMElemProxyRequest(string("GetComp,1,") + Ifu::FromInt(aInd));
};

void *MelemPx::DoGetObj(const char *aName)
{
    return NewProxyRequest(string("DoGetObj,1,") + aName, aName);
}

void MelemPx::SaveChromo(const char* aPath) const {}

TBool MelemPx::IsProvided() const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "IsProvided", resp);
    if (rr) 
	res = Ifu::ToBool(resp);
    return res;
}

string MelemPx::GetChromoSpec() const
{
    string resp;
    TBool rr = mMgr->Request(mContext, "GetChromoSpec", resp);
    if (rr) {
	return resp;
    } else {
	return string();
    }
}

string MelemPx::Mid() const
{
    return mMgr->Oid();
}

string MelemPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}


MIface* MelemPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MElem::Type()) {
	res = (MElem*) this;
    }
    return res;
}

const MIface* MelemPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MElem::Type()) {
	res = (const MElem*) this;
    }
    return res;
}

void* MelemPx::GetIfind(const string& aName, const TICacheRCtx& aCtx, TInt aInd)
{
    string resp;
    string req("GetIfind,1,");
    req += aName + Ifu::KRinvSep;
    string ctx;
    EIfu::FromCtx(aCtx, ctx);
    req += ctx;
    req += Ifu::KRinvSep + Ifu::FromInt(aInd);
    MIface* res = (MIface*) NewProxyRequest(req, aName);
    return res;
}

void MelemPx::UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx)
{
    string resp;
    string req("UnregIfReq,1,");
    req += aIfName + Ifu::KRinvSep;
    string ctx;
    EIfu::FromCtx(aCtx, ctx);
    req += ctx;
    mMgr->Request(mContext, req, resp);
}

void MelemPx::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv)
{
    string resp;
    string req("UnregIfProv,1,");
    req += aIfName;
    string ctx;
    EIfu::FromCtx(aCtx, ctx);
    req += Ifu::KRinvSep;
    req += ctx;
    string prov = aProv->GetUri(NULL, ETrue);
    req += Ifu::KRinvSep;
    req += prov;
    string inv = Ifu::FromBool(aInv);
    req += Ifu::KRinvSep;
    req += inv;
    mMgr->Request(mContext, req, resp);
}

void MelemPx::DumpChilds() const
{
    __ASSERT(false);
}

void MelemPx::DumpContent() const
{
    __ASSERT(false);
}

TBool MelemPx::ContValueExists(const string& aName) const
{
    TBool res = EFalse;
    string resp;
    TBool rr = mMgr->Request(mContext, "ContValueExists,1," + aName, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

TBool MelemPx::IsContOfCategory(const string& aName, const string& aCategory) const
{
    __ASSERT(EFalse);
    return EFalse;
}

void MelemPx::DumpComps() const
{
    __ASSERT(EFalse);
}

string MelemPx::GetAssociatedData(const string& aUri) const
{
    __ASSERT(EFalse);
}

MElem* MelemPx::GetCtx()
{
    return NewMElemProxyRequest("GetCtx");
}

void MelemPx::SetCtx(MElem* aOwner)
{
    __ASSERT(EFalse);
}

