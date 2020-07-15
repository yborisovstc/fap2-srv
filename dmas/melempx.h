#ifndef _MELEMPX_h_
#define _MELEMPX_h_

#include "daaproxy.h"
#include <melem.h>
#include <chromo.h>
#include <ifu.h>

class TMut;

/** @brief MElem interface proxy
 * */
class MelemPx : public DaaProxy, public MElem
{
    public:
	MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MelemPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
	// From MElem.MIface	
	virtual MIface* MElem_Call(const string& aSpec, string& aRes) override;
	virtual string MElem_Mid() const override;
	virtual string Uid() const override;
    public:
	// From MElem
	virtual MUnit* GetUnit() override;
	virtual void GetCRoot(TMut& aMut) const override;
	virtual MUnit* GetInhRoot() const override;
	virtual void Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual void Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual MUnit* GetAowner() override;
	virtual const MUnit* GetAowner() const override;
	virtual MUnit* GetAcompOwning(MUnit* aComp) override;
	virtual MUnit* GetUpperAowner() override;
	virtual MUnit* GetAttachedMgr();
	virtual const MUnit* GetAttachedMgr() const override;
	virtual TBool IsCompAttached(const MUnit* aComp) const override;
	virtual TBool IsAownerOf(const MUnit* aElem) const override;
	virtual void ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx) override;
	virtual TInt GetCompLrank(const MUnit* aComp) const override;
	virtual TBool IsInheritedComp(const MUnit* aNode) const override;
	virtual TBool HasInherDeps(const MUnit* aScope) const override;
	virtual MUnit* GetCompAowner(const MUnit* aComp) override;
	virtual const MUnit* GetCompAowner(const MUnit* aComp) const override;
	// Debugging
	virtual void SaveChromo(const char* aPath) const override;
	virtual void DumpChilds() const override;
	virtual void DumpCnode(const ChromoNode& aNode) const override;
    public:
	// MMUtable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual TBool DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual MUnit* AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx) override;
	virtual TBool RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx);
	virtual string GetChromoSpec() const override;
	virtual const MChromo& Chromos() const override;
	virtual MChromo& Chromos() override;
	virtual void SetMutation(const ChromoNode& aMuta) override;
	virtual TBool AppendMutation(const string& aFileName) override;
	virtual ChromoNode AppendMutation(const ChromoNode& aMuta) override;
	virtual ChromoNode AppendMutation(TNodeType aType) override;
	virtual void AppendMutation(const TMut& aMut) override;
	virtual TBool IsChromoAttached() const override;
	virtual TBool CompactChromo() override;
	virtual TBool CompactChromo(const ChromoNode& aNode) override;
	virtual void OnNodeMutated(const MUnit* aNode, const TMut& aMut, const MutCtx& aCtx) override;
	virtual void OnParentMutated(MUnit* aParent, const TMut& aMut) override;
    public:
	// MParent
	virtual void OnChildDeleting(MUnit* aChild) override;
	virtual TBool OnChildRenamed(MUnit* aComp, const string& aOldName) override;
	virtual TBool AppendChild(MUnit* aChild) override;
	virtual void RemoveChild(MUnit* aChild) override;
    public:
	// MChild
	virtual MUnit* GetParent() override;
	virtual const MUnit* GetParent() const override;
	virtual void OnParentDeleting(MUnit* aParent) override;
	virtual void SetParent(const string& aParent) override;
	virtual void SetParent(MUnit* aParent);
    protected:
	MElem* NewMElemProxyRequest(const string& aCallSpec);
	const MElem* NewMElemProxyRequest(const string& aCallSpec) const;
    protected:
	MChromo* mChromo; /*!< Cached chromo */
};

#endif 
