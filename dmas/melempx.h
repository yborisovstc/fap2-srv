#ifndef _MELEMPX_h_
#define _MELEMPX_h_

#include "daaproxy.h"
#include <melem.h>
#include <chromo.h>

/*
 * MElem proxy
 */

class MelemPx : public DaaProxy, public MElem
{
    public:
	MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MelemPx();
    public:
	// From MProxy
	virtual const string& GetCursor() const;
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
    public:
	// From MElem
	virtual void *DoGetObj(const char *aName);
	virtual const string EType(TBool aShort = ETrue) const;
	virtual const string& Name() const;
	virtual MElem* GetMan();
	virtual const MElem* GetMan() const;
	virtual void SetMan(MElem* aMan);
	virtual void SetObserver(MCompsObserver* aObserver);
	virtual vector<MElem*>& Comps();
	virtual const vector<MElem*>& Comps() const;
	virtual MElem* GetNode(const string& aUri);
	virtual MElem* GetNode(const GUri& aUri);
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse);
	virtual MElem* GetRoot() const;
	virtual MElem* GetInhRoot() const;
	virtual TInt GetContCount() const;
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	virtual void GetCont(string& aCont, const string& aName=string()); 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue);
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = ETrue, TBool aTrialMode = ETrue);
	// Gets URI from hier top node aTop, if aTop is NULL then the absolute URI will be produced
	virtual void GetUri(GUri& aUri, MElem* aTop = NULL) const;
	virtual void GetRUri(GUri& aUri, MElem* aTop = NULL);
	virtual string GetUri(MElem* aTop = NULL, TBool aShort = EFalse) const;
	virtual string GetRUri(MElem* aTop = NULL);
	virtual TBool RebaseUri(const GUri& aUri, const MElem* aBase, GUri& aRes);
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MElem* aBase, GUri& aRes);
	virtual MElem* GetAowner();
	virtual const MElem* GetAowner() const;
	virtual MElem* GetAcompOwning(MElem* aComp);
	virtual MElem* GetUpperAowner();
	virtual MElem* GetCommonOwner(MElem* aElem);
	virtual TBool IsRemoved() const;
	virtual TBool IsPhenoModif() const;
	virtual void SetRemoved();
	virtual MElem* GetAttachingMgr();
	virtual const MElem* GetAttachingMgr() const;
	virtual TBool IsAownerOf(const MElem* aElem) const;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const;
	virtual void GetLRank(Rank& aRank, TBool aCur = EFalse) const; 
	// Gets the comp with given type and owning given element
	virtual MElem* GetCompOwning(const string& aParent, MElem* aElem);
	virtual MElem* GetCompOwning(MElem* aElem);
	virtual TBool IsInheritedComp(const MElem* aNode) const;
	virtual TBool HasInherDeps(const MElem* aScope) const;
	// Debugging
	virtual TInt GetCapacity() const;
	virtual TBool IsHeirOf(const string& aParent) const;
    public:
	// MMUtable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual MElem* CreateHeir(const string& aName, MElem* aMan);
	virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse);
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse);
	virtual const MChromo& Chromos() const;
	virtual MChromo& Chromos();
	virtual MChromo& Mutation();
	virtual void SetMutation(const ChromoNode& aMuta);
	virtual TBool AppendMutation(const string& aFileName);
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta);
	virtual TMDeps& GetMDeps();
	virtual void AddMDep(MElem* aNode, const ChromoNode& aMut, TNodeAttr aAttr);
	virtual void AddCMDep(const ChromoNode& aMut, TNodeAttr aAttr, MElem* aNode);
	virtual void RemoveMDep(const TMDep& aDep, const MElem* aContext = NULL);
	virtual TBool RmCMDep(const ChromoNode& aMut, TNodeAttr aAttr, const MElem* aContext = NULL);
	virtual TBool IsChromoAttached() const;
	virtual void GetDep(TMDep& aDep, TNodeAttr aAttr, TBool aLocalOnly = EFalse, TBool aAnyType = EFalse) const;
	virtual TMDep GetMajorDep();
	virtual void GetMajorDep(TMDep& aDep, TBool aUp = EFalse, TBool aDown = ETrue);
	virtual TMDep GetMajorDep(TNodeType aMut, MChromo::TDepsLevel aLevel);
	virtual void GetMajorDep(TMDep& aDep, TNodeType aMut, MChromo::TDPath aDpath, MChromo::TDepsLevel aLevel, TBool aUp = ETrue, TBool aDown = ETrue);
	virtual ChromoNode GetLocalForwardCCDep(MElem* aOwner, const ChromoNode& aMut) const;
	virtual void GetImplicitDep(TMDep& aDep, MElem* aObj, MElem* aRef);
	virtual TBool IsRefSafe(MElem* aRef, TNodeAttr aReftype, MElem* aObj = NULL, TMDep* aDep = NULL);
	// Resolve owned mutation unsafety via changing mutation position
	virtual TBool ResolveMutUnsafety(MElem* aMutated, const TMDep& aDep);
	virtual TBool ResolveMutsUnsafety();
	virtual TBool CompactChromo();
	virtual TBool CompactChromo(const ChromoNode& aNode);
	// Transformations
	virtual TBool HasParentModifs() const;
	virtual void CopyModifsFromParent();
	virtual TBool HasModifs(const MElem* aOwner) const;
	virtual void CopyParentModifsToComp(MElem* aComp);
	virtual TBool RebaseUriToIntNode(const GUri& aUri, const MElem* aComp, GUri& aResult);
    public:
	// MCompsObserver
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual void OnCompAdding(MElem& aComp);
	virtual TBool OnCompChanged(MElem& aComp);
	virtual TBool OnContentChanged(MElem& aComp);
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
    public:
	// MOwner
	virtual TBool IsComp(const MElem* aElem) const;
	virtual TBool MoveComp(MElem* aComp, MElem* aDest);
	virtual TBool MoveComp(MElem* aComp, const ChromoNode& aDest);
	virtual TBool AppendComp(MElem* aComp);
    public:
	// MParent
	virtual void OnChildDeleting(MElem* aChild);
	virtual TBool OnChildRenamed(MElem* aComp, const string& aOldName);
	virtual TBool AppendChild(MElem* aChild);
	virtual void RemoveChild(MElem* aChild);
    public:
	// MChild
	virtual MElem* GetParent();
	virtual const MElem* GetParent() const;
	virtual void OnParentDeleting(MElem* aParent);
	virtual void SetParent(const string& aParent);
	virtual void SetParent(MElem* aParent);
    public:
	// MIfProv
	virtual void* GetSIfiC(const string& aName, Base* aRequestor);
	virtual void* GetSIfi(const string& aName, const RqContext* aCtx);
	virtual void* GetSIfi(const string& aReqUri, const string& aName, TBool aReqAssert);
	virtual TIfRange GetIfi(const string& aName, const RqContext* aCtx);
    public:
	// From MIface
	virtual MIface* Call(const string& aSpec, string& aRes);
    protected:
	Chromo* mChromo;
	TMDeps mMDeps;
	string mName;
};

#endif 
