#ifndef _MELEMPX_h_
#define _MELEMPX_h_

#include "daaproxy.h"
#include <melem.h>
#include <chromo.h>
#include <ifu.h>

/*
 * MElem proxy
 */

class MelemPx : public DaaProxy, public MElem
{
    public:
	MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MelemPx();
    public:
	virtual DaaProxy* CreateProxy(const string& aId, const string& aContext) const;
	virtual void *GetIface(const string& aName);
	virtual const void *GetIface(const string& aName) const;
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
    public:
	// From MElem
	virtual void *DoGetObj(const char *aName);
	virtual const string EType(TBool aShort = ETrue) const;
	virtual const string& Name() const;
	virtual TBool IsProvided() const;
	virtual MElem* GetMan();
	virtual const MElem* GetMan() const;
	virtual void SetMan(MElem* aMan);
	virtual void SetObserver(MCompsObserver* aObserver);
	virtual vector<MElem*>& Comps();
	virtual const vector<MElem*>& Comps() const;
	virtual MElem* GetComp(const string& aParent, const string& aName);
	virtual MElem* GetComp(const string& aParent, const string& aName) const;
	virtual MElem* GetNode(const string& aUri, TBool aInclRm = EFalse);
	virtual MElem* GetNode(const GUri& aUri, TBool aInclRm = EFalse);
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse);
	virtual MElem* GetRoot() const;
	virtual MElem* GetInhRoot() const;
	virtual TInt GetContCount() const;
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	virtual void GetCont(string& aCont, const string& aName=string()); 
	virtual TBool GetCont(TInt aInd, string& aName, string& aCont) const;
	virtual string GetContent(const string& aName=string()) const;
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()); 
	virtual TBool MoveNode(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode = EFalse);
	virtual void Mutate(TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL);
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly = EFalse, TBool aCheckSafety = EFalse, TBool aTrialMode = ETrue, const MElem* aCtx = NULL);
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
	virtual void SetRemoved();
	virtual MElem* GetAttachedMgr();
	virtual const MElem* GetAttachedMgr() const;
	virtual TBool IsCompAttached(const MElem* aComp) const;
	virtual TBool IsAownerOf(const MElem* aElem) const;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal);
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual void GetRank(Rank& aRank) const;
	virtual void GetRank(Rank& aRank, const ChromoNode& aMut) const;
	virtual void GetCompRank(Rank& aRank, const MElem* aComp) const;
	virtual TInt GetCompLrank(const MElem* aComp) const;
	// Gets the comp with given type and owning given element
	virtual MElem* GetCompOwning(const string& aParent, MElem* aElem);
	virtual MElem* GetCompOwning(MElem* aElem);
	virtual const MElem* GetCompOwning(const MElem* aElem) const;
	virtual TBool IsInheritedComp(const MElem* aNode) const;
	virtual TBool HasInherDeps(const MElem* aScope) const;
	virtual MElem* GetCompAowner(const MElem* aComp);
	virtual const MElem* GetCompAowner(const MElem* aComp) const;
	// Debugging
	virtual TInt GetCapacity() const;
	virtual TBool IsHeirOf(const string& aParent) const;
	virtual void DumpMcDeps() const;
	virtual void DumpCmDeps() const;
	virtual MElem* GetNodeS(const char* aUri);
	virtual MElem* GetComp(TInt aInd);
	virtual void SaveChromo(const char* aPath) const;
    public:
	// MMUtable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual MElem* CreateHeir(const string& aName, MElem* aMan);
	virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual const MChromo& Chromos() const;
	virtual MChromo& Chromos();
	virtual MChromo& Mutation();
	virtual auto_ptr<MChromo> GetFullChromo() const;
	virtual string GetChromoSpec() const;
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
	virtual TBool CompactChromo();
	virtual TBool CompactChromo(const ChromoNode& aNode);
	virtual void OnNodeMutated(const MElem* aNode, const ChromoNode& aMut, const MElem* aCtx);
	virtual void OnParentMutated(MElem* aParent, const ChromoNode& aMut);
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
	virtual TBool RegisterChild(const string& aChildUri);
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
    protected:
	void* NewProxyRequest(const string& aCallSpec, const string& aPxType);
	const void* NewProxyRequest(const string& aCallSpec, const string& aPxType) const;
	MElem* NewMElemProxyRequest(const string& aCallSpec);
	const MElem* NewMElemProxyRequest(const string& aCallSpec) const;
    protected:
	Chromo* mChromo;
	TMDeps mMDeps;
	string mName;
	/*
    protected:
	class EIfu: public Ifu {
	    public:
		EIfu();
	};
	// Interface methods utility
	static EIfu mIfu;
	*/
};

#endif 
