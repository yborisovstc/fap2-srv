#ifndef _MELEMPX_h_
#define _MELEMPX_h_

#include "daaproxy.h"
#include <melem.h>
#include <chromo.h>
#include <ifu.h>

class TMut;
/*
 * MElem proxy
 */

class MelemPx : public DaaProxy, public MElem
{
    public:
	class IfIter: public MIfProv::MIfIter {
	    friend class Elem;
	    public:
	    IfIter(): mHost(NULL) {};
	    IfIter(MelemPx* aHost, const string& aIfName, const TICacheRCtx& aReq, int aInd);
	    IfIter(const IfIter& aIt);
	    virtual MIfIter* Clone() const;
	    virtual MIfIter& operator=(const MIfIter& aIt);
	    virtual MIfIter& operator++();
	    virtual TBool operator==(const MIfIter& aIt);
	    virtual void*  operator*();
	    public:
	    MelemPx* mHost;
	    string mIfName; // Iface name
	    TICacheRCtx mReq; // Requestor
	    int mInd; // Index of iface in the range
	};

    public:
	MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MelemPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
    public:
	// From MElem
	virtual void Delete();
	virtual void *DoGetObj(const char *aName);
	virtual string EType(TBool aShort = ETrue) const;
	virtual const string& Name() const;
	virtual TBool IsProvided() const;
	virtual MElem* GetMan();
	virtual const MElem* GetMan() const;
	virtual void GetCRoot(TMut& aMut) const;
	virtual void SetMan(MElem* aMan);
	virtual void SetObserver(MAgentObserver* aObserver);
	//virtual MElem* GetComp(const string& aParent, const string& aName);
	virtual MElem* GetComp(const string& aParent, const string& aName) const;
	virtual MElem* GetNode(const string& aUri, TBool aInclRm = EFalse);
	virtual MElem* GetNode(const GUri& aUri, TBool aInclRm = EFalse);
	virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse);
	virtual MElem* GetRoot() const;
	virtual MElem* GetInhRoot() const;
	virtual TInt GetContCount(const string& aName = string()) const;
	virtual TBool ContentExists(const string& aName) const;
	virtual TBool ContValueExists(const string& aName=string()) const;
	virtual TBool IsContOfCategory(const string& aName, const string& aCategory) const; 
	virtual string GetContComp(const string& aOwnerName, TInt aInd) const;
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const; 
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
	virtual void SetRemoved(TBool aModif);
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
	virtual MElem* GetCtx();
	virtual void SetCtx(MElem* aOwner);
	// Debugging
	virtual TInt GetCapacity() const;
	virtual TBool IsHeirOf(const string& aParent) const;
	virtual void DumpMcDeps() const;
	virtual void DumpCmDeps() const;
	virtual void DumpIfReqs() const override {}
	virtual MElem* GetNodeS(const char* aUri);
	virtual TInt CompsCount() const;
	virtual MElem* GetComp(TInt aInd);
	virtual void SaveChromo(const char* aPath) const;
	virtual void DumpChilds() const;
	virtual void DumpComps() const;
	virtual void DumpContent() const;
	// Visual client debugging, ref ds_visdbg
	virtual string GetAssociatedData(const string& aUri) const;
    public:
	// MMUtable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual MElem* CreateHeir(const string& aName, MElem* aMan, MElem* aContext);
	virtual MElem* AddElem(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	virtual const MChromo& Chromos() const;
	virtual MChromo& Chromos();
	virtual unique_ptr<MChromo> GetFullChromo() const;
	virtual string GetChromoSpec() const;
	virtual void SetMutation(const ChromoNode& aMuta);
	virtual TBool AppendMutation(const string& aFileName);
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta);
	virtual ChromoNode AppendMutation(TNodeType aType);
	virtual void AppendMutation(const TMut& aMut);
	virtual TBool IsChromoAttached() const;
	virtual TBool CompactChromo();
	virtual TBool CompactChromo(const ChromoNode& aNode);
	virtual void OnNodeMutated(const MElem* aNode, const TMut& aMut, const MElem* aCtx);
	virtual void OnParentMutated(MElem* aParent, const TMut& aMut);
    public:
	// MCompsObserver
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue, TBool aModif = EFalse);
	virtual void OnCompAdding(MElem& aComp, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string(), TBool aModif = EFalse);
	virtual TBool OnChanged(MElem& aComp);
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
    public:
	// MOwner
	virtual TBool IsComp(const MElem* aElem) const;
	virtual TBool AppendComp(MElem* aComp, TBool aRt = EFalse);
	virtual void RemoveComp(MElem* aComp);
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
	void* GetIfind(const string& aName, const TICacheRCtx& aCtx, TInt aInd);
	virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx);
	virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MElem* aProv, TBool aInv = EFalse);
    protected:
	MElem* NewMElemProxyRequest(const string& aCallSpec);
	const MElem* NewMElemProxyRequest(const string& aCallSpec) const;
    protected:
	Chromo* mChromo;
	string mName;
};

#endif 
