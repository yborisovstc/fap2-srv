
#include <mprov.h>
#include <elem.h>
#include "melempx.h"
#include <stdexcept> 

const string FakeString = "";
vector<MElem*> FakeComps;

/*
MelemPx::EIfu MelemPx::mIfu;

// Ifu static initialisation
MelemPx::EIfu::EIfu()
{
    RegMethod("Name", 0);
    RegMethod("GetMan", 0);
    RegMethod("GetNode", 1);
    RegMethod("GetCont", 1);
    RegMethod("Mutate", 4);
}
*/


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

string MelemPx::Uid() const
{
    return GetContext();
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
    } else if (name == "GetRoot") {
	res = GetRoot();
    } else if (name == "GetNode") {
	res = GetNode(args.at(0));
    } else if (name == "GetCont") {
	GetCont(aRes, args.at(0));
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }
    return res;
}

MElem* MelemPx::NewProxyRequest(const string& aCallSpec)
{ 
    MElem* res = NULL;
    string resp;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	if (!IsCached(resp)) {
	    MelemPx* px = new MelemPx(mEnv, this, resp);
	    RegProxy(px);
	    res = px;
	} else {
	    res = dynamic_cast<MElem*>(GetProxy(resp));
	}
    }
    return res;
}

MElem* MelemPx::NewProxyRequest(const string& aCallSpec) const
{
    MElem* res = NULL;
    string resp;
    MelemPx* tis = (MelemPx*) this;
    TBool rres = mMgr->Request(mContext, aCallSpec, resp);
    if (rres) {
	if (!IsCached(resp)) {
	    MelemPx* px = new MelemPx(mEnv, tis, resp);
	    tis->RegProxy(px);
	    res = px;
	} else {
	    res = dynamic_cast<MElem*>(tis->GetProxy(resp));
	}
    }
    return res;
}

const string MelemPx::EType(TBool aShort) const { return string();}

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
    return NewProxyRequest("GetMan");
}

const MElem* MelemPx::GetMan() const { return NULL;}

void MelemPx::SetMan(MElem* aMan) {}

void MelemPx::SetObserver(MCompsObserver* aObserver) {}

vector<MElem*>& MelemPx::Comps() { return FakeComps;}

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
    }
    return res;
}

MElem* MelemPx::GetRoot() const
{
    return NewProxyRequest("GetRoot");
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

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) {}

void MelemPx::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) {}

void MelemPx::GetUri(GUri& aUri, MElem* aTop) const {}

void MelemPx::GetRUri(GUri& aUri, MElem* aTop) {}

string MelemPx::GetUri(MElem* aTop, TBool aShort) const { return string();}

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

MElem* MelemPx::GetAttachingMgr() {return NULL;}

const MElem* MelemPx::GetAttachingMgr() const {return NULL;}

TBool MelemPx::IsAownerOf(const MElem* aElem) const {return false;}

TBool MelemPx::ChangeAttr(TNodeAttr aAttr, const string& aVal) {return false;}

void MelemPx::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) {}

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

MElem* MelemPx::GetComp(const string& aParent, const string& aName) {};

MElem* MelemPx::GetComp(const string& aParent, const string& aName) const {};



void MelemPx::DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) {}

TBool  MelemPx::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) { return false;}

MElem*  MelemPx::CreateHeir(const string& aName, MElem* aMan)
{
    MElem* heir = NULL;
    if (Provider()->IsProvided(this)) {
	heir = Provider()->CreateNode(Name(), aName, aMan, mEnv);
    }
    else {
	MElem* parent = GetParent();
	__ASSERT(parent != NULL);
	MElem* owner = GetMan();
	__ASSERT(owner != NULL);
	if (Provider()->IsProvided(parent)) {
	    // Parent is Agent - native element. Create via provider
	    heir = Provider()->CreateNode(EType(), aName, owner, mEnv);
	}
	else {
	    heir = parent->CreateHeir(aName, owner);
	}
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	auto_ptr<MChromo> chr = GetFullChromo();
	ChromoNode& root = chr->Root();
	heir->SetMutation(root);
	// Mutate run-time only - !! DON'T UPDATE CHROMO, ref UC_019
	heir->Mutate(ETrue, EFalse, EFalse, EFalse);
	// Mutate bare child with original parent chromo, mutate run-time only to have clean heir's chromo
	// Mutated with parent's own chromo - so panent's name is the type now. Set also the parent, but it will be updated further
	heir->SetParent(Name());
	// Relocate heir to hier from which the request of creating heir came
	heir->SetMan(NULL);
	heir->SetMan(aMan);
	// Re-adopte the child
	parent->RemoveChild(heir);
	AppendChild(heir);
    }
    return heir;
}

MElem*  MelemPx::AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, TBool aAttach) {return NULL;}

TBool  MelemPx::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, TBool aAttach) { return false;}

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

void MelemPx::OnNodeMutated(const MElem* aNode, const ChromoNode& aMut) {};

void MelemPx::OnParentMutated(MElem* aParent, const ChromoNode& aMut) {};




void  MelemPx::OnCompDeleting(MElem& aComp, TBool aSoft) {}

void  MelemPx::OnCompAdding(MElem& aComp) {}

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

TBool  MelemPx::AppendChild(MElem* aChild) { return false;}

void  MelemPx::RemoveChild(MElem* aChild) {}


// MChild

MElem*  MelemPx::GetParent() { return NULL;}

const MElem*  MelemPx::GetParent() const { return NULL;}

void  MelemPx::OnParentDeleting(MElem* aParent) {}

void  MelemPx::SetParent(const string& aParent) {}

void  MelemPx::SetParent(MElem* aParent) {}


// MIfProv

void*  MelemPx::GetSIfiC(const string& aName, Base* aRequestor) { return NULL;}

void*  MelemPx::GetSIfi(const string& aName, const RqContext* aCtx) { return NULL;}

void*  MelemPx::GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert) { return NULL;}

MElem::TIfRange  MelemPx::GetIfi(const string& aName, const RqContext* aCtx) { return TIfRange(TIfIter(), TIfIter());}

// Debugging

void MelemPx::DumpMcDeps() const {};

void MelemPx::DumpCmDeps() const {};

MElem* MelemPx::GetNodeS(const char* aUri) {};

MElem* MelemPx::GetComp(TInt aInd) {};

void *MelemPx::DoGetObj(const char *aName) { return NULL;}
