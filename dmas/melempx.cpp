
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

MIface*  MelemPx::IfIter::operator*()
{
    MIface* res = (MIface*) mHost->GetIfind(mIfName, mReq, mInd);
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
    if (!args_ok) {
	    throw (runtime_error("Wrong arguments number"));
	    /*
    }
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
	*/
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

string MelemPx::EType(TBool aShort) const
{
    return Rpc<string, TBool>(__func__, aShort);
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
    return RpcPx<MElem>(__func__);
}

const MElem* MelemPx::GetMan() const
{
    return RpcPx<MElem>(__func__);
}

void MelemPx::GetCRoot(TMut& aMut) const
{
    aMut = Rpc<string>(__func__);
}

// Ignoring. Only one way relation is allowed for the proxy
// TODO [YB] To check if assert makes sense here
void MelemPx::SetMan(MElem* aMan)
{
}

void MelemPx::SetObserver(MAgentObserver* aObserver)
{
    Rpcv<MIface*>(__func__, aObserver);
}

MElem* MelemPx::GetNode(const string& aUri, TBool aInclRm) 
{
    return RpcPx<MElem>(__func__, aUri, aInclRm);
}

MElem* MelemPx::GetNode(const GUri& aUri, TBool aInclRm)
{
    return GetNode(aUri.toString(ETrue), aInclRm);
}

// TODO [YB] Implicit reduction of args is used here (aUri is not sent). To check for correctness.
MElem* MelemPx::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm)
{ 
    MElem* res = NULL;
    string suri = aUri.toString(aPathBase);
    string cspec = "GetNode#2,1," + suri + "," + Ifu::FromBool(aAnywhere) + "," + Ifu::FromBool(aInclRm);
    res = NewMElemProxyRequest(cspec);
    return res;
}

MElem* MelemPx::GetRoot() const
{
    return RpcPx<MElem>(__func__);
}

MElem* MelemPx::GetInhRoot() const
{
    __ASSERT(false);
    return NULL;
}

TInt MelemPx::GetContCount(const string& aName) const
{
    return Rpc<TInt, const string&>(__func__, aName);
}

TBool MelemPx::ContentExists(const string& aName) const
{
    return Rpc<TBool, const string&>(__func__, aName);
}
 
string MelemPx::GetContComp(const string& aOwnerName, TInt aInd) const
{
    return Rpc<string, const string&, TInt>(__func__, aOwnerName, aInd);
}

string MelemPx::GetContent(const string& aName, TBool aFull) const
{
    return Rpc<string, const string&, TBool>(__func__, aName, aFull);
}

TBool MelemPx::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) {return false;}
 
TBool MelemPx::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode) {return false;}

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    Rpcv<TBool, TBool, TBool, const MElem*>("Mutate#2", aRunTimeOnly, aCheckSafety, aTrialMode, aCtx);
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
    return Rpc<string, MElem*>(__func__, aTop);
}

string MelemPx::GetRUri(MElem* aTop)
{
    return Rpc<string, MElem*>(__func__, aTop);
}

TBool MelemPx::RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes) {return false;}

TBool MelemPx::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes) {return false;}

MElem* MelemPx::GetAowner()
{
    return RpcPx<MElem>(__func__);
}

const MElem* MelemPx::GetAowner() const
{
    return RpcPx<const MElem>(__func__);
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
    return Rpc<TBool>(__func__);
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
    return Rpc<TInt, const MElem*>(__func__, aComp);
}
 
MElem* MelemPx::GetCompOwning(const string& aParent, MElem* aElem)
{
    __ASSERT(false);
    return NULL;
}

MElem* MelemPx::GetCompOwning(MElem* aElem)
{
    return RpcPx<MElem, MElem*>(__func__, aElem);
}

const MElem* MelemPx::GetCompOwning(const MElem* aElem) const
{
    return RpcPxC<MElem, const MElem*>(__func__, aElem);
}

TBool MelemPx::IsInheritedComp(const MElem* aNode) const
{
    __ASSERT(false);
    return false;
}

MElem* MelemPx::GetCompAowner(const MElem* aComp)
{
    return RpcPx<MElem, const MElem*>(__func__, aComp);
}

const MElem* MelemPx::GetCompAowner(const MElem* aComp) const
{
    return RpcPxC<MElem, const MElem*>(__func__, aComp);
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
    return Rpc<TBool, string>(__func__, aParent);
}

TInt MelemPx::CompsCount() const
{
    return Rpc<TInt>(__func__);
}

MElem* MelemPx::GetComp(const string& aParent, const string& aName) const
{
    // TODO [YB] To implement auto-generation of method signature (to avoid suffix #2)
    return RpcPx<MElem>("GetComp#2", aParent, aName);
}

TBool MelemPx::IsCompAttached(const MElem* aComp) const
{
    return Rpc<TBool, const MElem*>(__func__, aComp);
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
    Rpcv<string>(__func__, aMut);
}

TBool  MelemPx::IsChromoAttached() const
{
    return Rpc<TBool>(__func__);
}

unique_ptr<MChromo> MelemPx::GetFullChromo() const
{
    __ASSERT(false);
};

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
    Rpcv<const MElem*, string, const MElem*>(__func__, aNode, aMut, aCtx);
}

void MelemPx::OnParentMutated(MElem* aParent, const TMut& aMut)
{
    __ASSERT(false);
};

void  MelemPx::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    // TODO Error on templ args deduction when passing MElem&, to analyze
    Rpcv<MElem*, TBool>(__func__, &aComp, aSoft);
}

void  MelemPx::OnCompAdding(MElem& aComp, TBool aModif)
{
    Rpcv<MElem*>(__func__, &aComp);
}

// TODO [YB] aModif isn't used anymore, to cleanup
TBool  MelemPx::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    return Rpc<TBool, MElem*, string>(__func__, &aComp, aContName);
}

TBool  MelemPx::OnChanged(MElem& aComp)
{
    return Rpc<TBool, MElem*>(__func__, &aComp);
}

TBool MelemPx::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    return Rpc<TBool, MElem*, string>(__func__, &aComp, aOldName);
}

TBool  MelemPx::IsComp(const MElem* aElem) const
{
    return Rpc<TBool, const MElem*>(__func__, aElem);
}

TBool  MelemPx::AppendComp(MElem* aComp, TBool aRt)
{
    return Rpc<TBool, MElem*, TBool>(__func__, aComp, aRt);
}

void MelemPx::RemoveComp(MElem* aComp)
{
    Rpcv<MElem*>(__func__, aComp);
}

// MParent
	
void  MelemPx::OnChildDeleting(MElem* aChild)
{
    Rpcv<MElem*>(__func__, aChild);
}

TBool  MelemPx::OnChildRenamed(MElem* aComp, const string& aOldName)
{
    __ASSERT(false);
    return false;
}

TBool  MelemPx::AppendChild(MElem* aChild)
{
    return Rpc<TBool, MElem*>(__func__, aChild);
}

void  MelemPx::RemoveChild(MElem* aChild)
{
    Rpcv<MElem*>(__func__, aChild);
}


// MChild

MElem* MelemPx::GetParent()
{
    return RpcPx<MElem>(__func__);
}

const MElem*  MelemPx::GetParent() const
{
    return RpcPx<const MElem>(__func__);
}

void  MelemPx::OnParentDeleting(MElem* aParent)
{
    Rpcv<MElem*>(__func__, aParent);
}

void  MelemPx::SetParent(const string& aParent)
{
    __ASSERT(false);
}

void  MelemPx::SetParent(MElem* aParent)
{
    Rpcv<MElem*>(__func__, aParent);
}


// MIfProv


MIface*  MelemPx::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert)
{
    __ASSERT(false);
    return NULL;
}

MElem::TIfRange  MelemPx::GetIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TInt ifcnt = Rpc<TInt, const string&, const TICacheRCtx&>(__func__, aName, aCtx);
    IfIter beg(this, aName, aCtx, 0);
    IfIter end(this, aName, aCtx, ifcnt);
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
    return RpcPx<MElem>(__func__, aInd);
};

void *MelemPx::DoGetObj(const char *aName)
{
    return RpcPxN(__func__, aName, aName);
}

void MelemPx::SaveChromo(const char* aPath) const {}

TBool MelemPx::IsProvided() const
{
    return Rpc<TBool>(__func__);
}

string MelemPx::GetChromoSpec() const
{
    return Rpc<string>(__func__);
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
    return RpcPxN(__func__, aName, aName, aCtx, aInd);
    /*
    string resp;
    string req("GetIfind,1,");
    req += aName + Ifu::KRinvSep;
    string ctx;
    EIfu::FromCtx(aCtx, ctx);
    req += ctx;
    req += Ifu::KRinvSep + Ifu::FromInt(aInd);
    MIface* res = (MIface*) NewProxyRequest(req, aName);
    return res;
    */
}

void MelemPx::UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx)
{
    Rpcv<const string&, const TICacheRCtx&>(__func__, aIfName, aCtx);
}

void MelemPx::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv)
{
    Rpcv<const string&, const TICacheRCtx&, MElem*, TBool>(__func__, aIfName, aCtx, aProv, aInv);
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
    return Rpc<TBool, const string&>(__func__, aName);
}

TBool MelemPx::IsContOfCategory(const string& aName, const string& aCategory) const
{
    return Rpc<TBool, const string&, const string&>(__func__, aName, aCategory);
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

