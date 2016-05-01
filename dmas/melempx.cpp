
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

MelemPx::IfIter::IfIter(const IfIter& aIt): mHost(aIt.mHost), mInd(aIt.mInd), mIfName(aIt.mIfName) {}

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
	throw (runtime_error("Unhandled method: " + name));
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
    TBool rr = mMgr->Request(mContext, "EType,1," + Ifu::FromBool(aShort), resp);
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

void MelemPx::SetMan(MElem* aMan) {}

void MelemPx::SetObserver(MAgentObserver* aObserver) {}

vector<MElem*>& MelemPx::Comps()
{ 
    return FakeComps;
}

const vector<MElem*>& MelemPx::Comps() const { return FakeComps;}

MElem* MelemPx::GetNode(const string& aUri, TBool aInclRm) 
{ 
    MElem* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, "GetNode,1," + aUri, resp);
    if (rres) {
	if (!IsCached(resp)) {
	    MelemPx* px = new MelemPx(mEnv, this, resp);
	    RegProxy(px);
	    res = px;
	} else {
	    res = dynamic_cast<MElem*>(GetProxy(resp));
	}
    } else {
	Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [GetNode %s] request failed", Uid().c_str(), aUri.c_str());
    }
    return res;
}

MElem* MelemPx::GetNode(const GUri& aUri, TBool aInclRm) { return NULL;}

MElem* MelemPx::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm)
{ 
    MElem* res = NULL;
    string suri = aUri.GetUri(aPathBase);
    string resp;
    TBool rres = mMgr->Request(mContext, "GetNode#2,1," + suri + "," + Ifu::FromBool(aAnywhere) + "," + Ifu::FromBool(aInclRm) , resp);
    if (rres) {
	if (!IsCached(resp)) {
	    MelemPx* px = new MelemPx(mEnv, this, resp);
	    RegProxy(px);
	    res = px;
	} else {
	    res = dynamic_cast<MElem*>(GetProxy(resp));
	}
    } else {
	Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [GetNode %s] request failed", Uid().c_str(), suri.c_str());
    }
    return res;
}

MElem* MelemPx::GetRoot() const
{
    return ((MelemPx*) this)->NewMElemProxyRequest("GetRoot");
}

MElem* MelemPx::GetInhRoot() const { return NULL;}

TInt MelemPx::GetContCount() const { return 0;}

TBool MelemPx::IsContChangeable(const string& aName) const {return false;}
 
// TODO [YB] Needs to return call status
void MelemPx::GetCont(string& aCont, const string& aName)
{
    mMgr->Request(mContext, "GetCont,1," + aName, aCont);
}
 
TBool MelemPx::GetCont(TInt aInd, string& aName, string& aCont) const {return false;}

string MelemPx::GetContent(const string& aName) const {};

TBool MelemPx::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) {return false;}
 
TBool MelemPx::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode) {return false;}

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) {}

void MelemPx::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) {}

void MelemPx::GetUri(GUri& aUri, MElem* aTop) const
{
    string uri = GetUri(aTop, ETrue);
    GUri guri(uri);
    aUri.Prepend(guri);
}

void MelemPx::GetRUri(GUri& aUri, MElem* aTop) {}

string MelemPx::GetUri(MElem* aTop, TBool aShort) const
{
    string res;
    if (aTop == NULL) {
	TBool rres = mMgr->Request(mContext, "GetUri,1", res);
	if (!rres) {
	    Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [GetUri] request failed: %s",
		    Uid().c_str(), res.c_str());
	}
    } else {
	string base;
	TBool rres = mMgr->Request(mContext, "GetUri,1", base);
	if (!rres) {
	    Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [GetUri] failed on getting base: %s",
		    Uid().c_str(), base.c_str());
	}
	rres = mMgr->Request(mContext, "GetUri#2,1," + base, res);
	if (!rres) {
	    Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [GetUri %s] failed on GetUri#2 request: %s",
		    Uid().c_str(), base.c_str(), res.c_str());
	}
    }
    return res;
}

string MelemPx::GetRUri(MElem* aTop) {return string();}

TBool MelemPx::RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes) {return false;}

TBool MelemPx::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes) {return false;}

MElem* MelemPx::GetAowner() {return NULL;}

const MElem* MelemPx::GetAowner() const {return NULL;}

MElem* MelemPx::GetAcompOwning(MElem* aComp) {return NULL;}

MElem* MelemPx::GetUpperAowner() {return NULL;}

MElem* MelemPx::GetCommonOwner(MElem* aElem) {return NULL;}

TBool MelemPx::IsRemoved() const {return false;}

void MelemPx::SetRemoved() {}

MElem* MelemPx::GetAttachedMgr() {return NULL;}

const MElem* MelemPx::GetAttachedMgr() const {return NULL;}

TBool MelemPx::IsAownerOf(const MElem* aElem) const {return false;}

TBool MelemPx::ChangeAttr(TNodeAttr aAttr, const string& aVal) {return false;}

void MelemPx::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) {}

void MelemPx::GetRank(Rank& aRank) const {};

void MelemPx::GetRank(Rank& aRank, const ChromoNode& aMut) const {}

void MelemPx::GetCompRank(Rank& aRank, const MElem* aComp) const {};

TInt MelemPx::GetCompLrank(const MElem* aComp) const {};
 
MElem* MelemPx::GetCompOwning(const string& aParent, MElem* aElem) {return NULL;}

MElem* MelemPx::GetCompOwning(MElem* aElem) {return NULL;}

const MElem* MelemPx::GetCompOwning(const MElem* aElem) const {};

TBool MelemPx::IsInheritedComp(const MElem* aNode) const {return false;}

MElem* MelemPx::GetCompAowner(const MElem* aComp) {};

const MElem* MelemPx::GetCompAowner(const MElem* aComp) const {};

TBool MelemPx::HasInherDeps(const MElem* aScope) const {return false;}

TInt MelemPx::GetCapacity() const {return 0;}

TBool MelemPx::IsHeirOf(const string& aParent) const {return false;}

TInt MelemPx::CompsCount() const
{
    TInt res = 0;
    string resp;
    TBool rr = mMgr->Request(mContext, "CompsCount", resp);
    if (rr)
	res = Ifu::ToInt(resp);
    return res;
}

MElem* MelemPx::GetComp(const string& aParent, const string& aName)
{
};

MElem* MelemPx::GetComp(const string& aParent, const string& aName) const {};

TBool MelemPx::IsCompAttached(const MElem* aComp) const {return EFalse;}


void MelemPx::DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) {}

TBool  MelemPx::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) { return false;}

MElem*  MelemPx::CreateHeir(const string& aName, MElem* aMan)
{
    MElem* heir = NULL;
    if (IsProvided()) {
	heir = Provider()->CreateNode(Name(), aName, aMan, mEnv);
    }
    else {
	MElem* parent = GetParent();
	__ASSERT(parent != NULL);
	MElem* owner = GetMan();
	__ASSERT(owner != NULL);
	if (parent->IsProvided()) {
	    // Parent is Agent - native element. Create via provider
	    heir = Provider()->CreateNode(EType(), aName, owner, mEnv);
	}
	else {
	    heir = parent->CreateHeir(aName, owner);
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
	heir->SetMan(NULL);
	heir->SetMan(aMan);
	// Re-adopte the child
	parent->RemoveChild(heir);
	AppendChild(heir);
	delete chr;
    }
    return heir;
}

MElem*  MelemPx::AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MElem* aCtx) {return NULL;}

TBool  MelemPx::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx) { return false;}

const MChromo&  MelemPx::Chromos() const { return *mChromo;}

MChromo&  MelemPx::Chromos() { return *mChromo;}

MChromo&  MelemPx::Mutation() { return *mChromo;}

void  MelemPx::SetMutation(const ChromoNode& aMuta) {}

TBool  MelemPx::AppendMutation(const string& aFileName) { return false;}

ChromoNode  MelemPx::AppendMutation(const ChromoNode& aMuta) { return ChromoNode(); }

TMDeps&  MelemPx::GetMDeps() { return mMDeps;}

void  MelemPx::AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr) {}

void  MelemPx::AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode) {}

void  MelemPx::RemoveMDep(const TMDep& aDep, const MElem* aContext) {}

TBool  MelemPx::RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext) { return false;}

TBool  MelemPx::IsChromoAttached() const { return false;}

auto_ptr<MChromo> MelemPx::GetFullChromo() const {};

void  MelemPx::GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly, TBool aAnyType) const {}

TMDep  MelemPx::GetMajorDep() { return mMDeps.at(0);}

void  MelemPx::GetMajorDep(TMDep& aDep, TBool aUp, TBool aDown) {}

TMDep  MelemPx::GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel) { return mMDeps.at(0);}

void  MelemPx::GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp, TBool aDown) {}

ChromoNode  MelemPx::GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const { return ChromoNode();}

TBool  MelemPx::CompactChromo() { return false;}

TBool  MelemPx::CompactChromo(const ChromoNode& aNode) { return false;}

void MelemPx::OnNodeMutated(const MElem* aNode, const ChromoNode& aMut, const MElem* aCtx) {}

void MelemPx::OnParentMutated(MElem* aParent, const ChromoNode& aMut) {};




void  MelemPx::OnCompDeleting(MElem& aComp, TBool aSoft) {}

void  MelemPx::OnCompAdding(MElem& aComp)
{
    string resp;
    string req("OnCompAdding,1,");
    string uri = aComp.GetUri(NULL, ETrue);
    req += uri;
    TBool res = mMgr->Request(mContext, req, resp);
    if (!res) {
	Logger()->Write(MLogRec::EErr, NULL, "Proxy [%s]: [OnCompAdding %s] request failed: %s",
		Uid().c_str(), uri.c_str(), resp.c_str());
    }
}

TBool  MelemPx::OnCompChanged(MElem& aComp) { return false;}

TBool  MelemPx::OnContentChanged(MElem& aComp) { return false;}

TBool  MelemPx::OnCompRenamed(MElem& aComp, const string& aOldName) { return false;}



TBool  MelemPx::IsComp(const MElem* aElem) const { return false;}

TBool  MelemPx::MoveComp(MElem* aComp, MElem* aDest) { return false;}

TBool  MelemPx::MoveComp(MElem* aComp, const ChromoNode& aDest) { return false;}

TBool  MelemPx::AppendComp(MElem* aComp) { return false;}


// MParent
	
void  MelemPx::OnChildDeleting(MElem* aChild) {}

TBool  MelemPx::OnChildRenamed(MElem* aComp, const string& aOldName) { return false;}

TBool  MelemPx::AppendChild(MElem* aChild)
{
    return EFalse;
    string uri = aChild->GetUri(this, ETrue);
    TBool res = RegisterChild(uri);
    if (res) {
	aChild->SetParent(this);
    }
    return res;
}

void  MelemPx::RemoveChild(MElem* aChild) {}


// MChild

MElem*  MelemPx::GetParent()
{
    return NewMElemProxyRequest("GetParent");
}

const MElem*  MelemPx::GetParent() const { return NULL;}

void  MelemPx::OnParentDeleting(MElem* aParent) {}

void  MelemPx::SetParent(const string& aParent) {}

void  MelemPx::SetParent(MElem* aParent) {}


// MIfProv

void*  MelemPx::GetSIfiC(const string& aName, Base* aRequestor)
{
    RqContext ctx(aRequestor);
    return GetSIfi(aName, &ctx);
}

void*  MelemPx::GetSIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    string resp;
    string req("GetSIfi,1,");
    req += aName + Ifu::KRinvSep;
    const RqContext* cct(aCtx);
    while (cct != NULL) {
	Base* rq = cct->Requestor();
	MElem* re = rq->GetObj(re);
	string reuri = re->GetUri(NULL, ETrue);
	req += reuri + Ifu::KArraySep;
	cct = cct->Ctx();
    }
    res = NewProxyRequest(req, aName);
    return res;
}

void*  MelemPx::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert) { return NULL;}

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

void MelemPx::DumpMcDeps() const {};

void MelemPx::DumpCmDeps() const {};

MElem* MelemPx::GetNodeS(const char* aUri) {};

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
    return resp;
}

TBool MelemPx::RegisterChild(const string& aChildUri)
{
    return EFalse;
}

string MelemPx::Uid() const
{
    return "Px%" + GetContext();
}

string MelemPx::Mid() const
{
    return string();
}

void *MelemPx::GetIface(const string& aName)
{
    void *res = NULL;
    if (aName == MElem::Type()) {
	res = (MElem*) this;
    }
    return res;
}

const void *MelemPx::GetIface(const string& aName) const
{
    const void *res = NULL;
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
    TBool rr = mMgr->Request(mContext, req, resp);
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
    TBool rr = mMgr->Request(mContext, req, resp);
}
