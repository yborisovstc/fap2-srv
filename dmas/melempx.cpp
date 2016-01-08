
#include <mprov.h>
#include "melempx.h"

const string FakeString = "";
vector<MElem*> FakeComps;

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

	

const string MelemPx::EType(TBool aShort) const { return string();}

const string& MelemPx::Name() const
{
    string resp;
    TBool res = mMgr->Request(mContext, "Name", resp);
    if (res) 
	((MelemPx*) this)->mName = resp;
    return mName;
}

MElem* MelemPx::GetMan() { return NULL;}

const MElem* MelemPx::GetMan() const { return NULL;}

void MelemPx::SetMan(MElem* aMan) {}

void MelemPx::SetObserver(MCompsObserver* aObserver) {}

vector<MElem*>& MelemPx::Comps() { return FakeComps;}

const vector<MElem*>& MelemPx::Comps() const { return FakeComps;}

MElem* MelemPx::GetNode(const string& aUri) 
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

MElem* MelemPx::GetNode(const GUri& aUri) { return NULL;}

MElem* MelemPx::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere) { return NULL;}

MElem* MelemPx::GetRoot() const { return NULL;}

MElem* MelemPx::GetInhRoot() const { return NULL;}

TInt MelemPx::GetContCount() const { return 0;}

TBool MelemPx::IsContChangeable(const string& aName) const {return false;}
 
// TODO [YB] Needs to return call status
void MelemPx::GetCont(string& aCont, const string& aName)
{
    mMgr->Request(mContext, "GetCont,1," + aName, aCont);
}
 
TBool MelemPx::GetCont(TInt aInd, string& aName, string& aCont) const {return false;}

TBool MelemPx::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) {return false;}
 
TBool MelemPx::MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode) {return false;}

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode) {}

void MelemPx::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode) {}

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

TBool MelemPx::IsPhenoModif() const {return false;}

void MelemPx::SetRemoved() {}

MElem* MelemPx::GetAttachingMgr() {return NULL;}

const MElem* MelemPx::GetAttachingMgr() const {return NULL;}

TBool MelemPx::IsAownerOf(const MElem* aElem) const {return false;}

TBool MelemPx::ChangeAttr(TNodeAttr aAttr, const string& aVal) {return false;}

void MelemPx::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode) {}

void MelemPx::GetRank(Rank& aRank, const ChromoNode& aMut) const {}

void MelemPx::GetLRank(Rank& aRank, TBool aCur) const {}
 
MElem* MelemPx::GetCompOwning(const string& aParent, MElem* aElem) {return NULL;}

MElem* MelemPx::GetCompOwning(MElem* aElem) {return NULL;}

TBool MelemPx::IsInheritedComp(const MElem* aNode) const {return false;}

TBool MelemPx::HasInherDeps(const MElem* aScope) const {return false;}

TInt MelemPx::GetCapacity() const {return 0;}

TBool MelemPx::IsHeirOf(const string& aParent) const {return false;}

MIface* MelemPx::Call(const string& aSpec, string& aRes) {return NULL;}




void MelemPx::DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode) {}

TBool  MelemPx::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode) { return false;}

MElem*  MelemPx::CreateHeir(const string& aName, MElem* aMan) {return NULL;}

MElem*  MelemPx::AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode) {return NULL;}

TBool  MelemPx::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode) { return false;}

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

void  MelemPx::GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly, TBool aAnyType) const {}

TMDep  MelemPx::GetMajorDep() { return mMDeps.at(0);}

void  MelemPx::GetMajorDep(TMDep& aDep, TBool aUp, TBool aDown) {}

TMDep  MelemPx::GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel) { return mMDeps.at(0);}

void  MelemPx::GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp, TBool aDown) {}

ChromoNode  MelemPx::GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const { return ChromoNode();}

void  MelemPx::GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef) {}

TBool  MelemPx::IsRefSafe(MElem* aRef, TNodeAttr aReftype, MElem* aObj, TMDep* aDep) { return false;}

TBool  MelemPx::ResolveMutUnsafety(MElem* aMutated, const TMDep& aDep) { return false;}

TBool  MelemPx::ResolveMutsUnsafety() { return false;}

TBool  MelemPx::CompactChromo() { return false;}

TBool  MelemPx::CompactChromo(const ChromoNode& aNode) { return false;}

TBool  MelemPx::HasParentModifs() const { return false;}

void  MelemPx::CopyModifsFromParent() {}

TBool  MelemPx::HasModifs(const MElem* aOwner) const { return false;}

void  MelemPx::CopyParentModifsToComp(MElem* aComp) {}

TBool  MelemPx::RebaseUriToIntNode(const GUri& aUri, const MElem* aComp, GUri& aResult) { return false;}





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



const string& MelemPx::GetCursor() const {}


void *MelemPx::DoGetObj(const char *aName) { return NULL;}
