#ifndef _MUNITPX_h_
#define _MUNITPX_h_

#include "daaproxy.h"
#include <munit.h>
#include <ifu.h>

/** @brief MUnit interface proxy
 * */
class MunitPx : public DaaProxy, public MUnit
{
    public:
	class IfIter: public MIfProv::MIfIter {
	    friend class Elem;
	    public:
	    IfIter(): mHost(NULL) {};
	    IfIter(MunitPx* aHost, const string& aIfName, const TICacheRCtx& aReq, int aInd);
	    IfIter(const IfIter& aIt);
	    virtual MIfIter* Clone() const;
	    virtual MIfIter& operator=(const MIfIter& aIt);
	    virtual MIfIter& operator++();
	    virtual TBool operator==(const MIfIter& aIt);
	    virtual MIface*  operator*();
	    public:
	    MunitPx* mHost;
	    string mIfName; // Iface name
	    TICacheRCtx mReq; // Requestor
	    int mInd; // Index of iface in the range
	};

    public:
	MunitPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MunitPx();
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
	// From MUnit
	virtual void Delete() override;
	virtual MIface *DoGetObj(const char *aName) override;
	virtual string EType(TBool aShort = ETrue) const override;
	virtual const string& Name() const override;
	virtual TBool IsProvided() const override;
	virtual MUnit* GetMan() override;
	virtual const MUnit* GetMan() const override;
	virtual void SetMan(MUnit* aMan) override;
	virtual MUnit* GetComp(const string& aParent, const string& aName) const override;
	virtual MUnit* GetNode(const string& aUri, TBool aInclRm = EFalse) override;
	virtual MUnit* GetNode(const GUri& aUri, TBool aInclRm = EFalse) override;
	virtual MUnit* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse, TBool aInclRm = EFalse) override;
	virtual MUnit* GetRoot() const override;
	virtual TInt GetContCount(const string& aName = string()) const override;
	virtual TBool ContentExists(const string& aName) const override;
	virtual TBool ContValueExists(const string& aName=string()) const override;
	virtual TBool IsContOfCategory(const string& aName, const string& aCategory) const override; 
	virtual string GetContComp(const string& aOwnerName, TInt aInd) const override;
	virtual string GetContent(const string& aName=string(), TBool aFull = EFalse) const override; 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName=string()) override; 
	virtual void GetUri(GUri& aUri, MUnit* aTop = NULL) const override;
	virtual void GetRUri(GUri& aUri, MUnit* aTop = NULL) override;
	virtual string GetUri(MUnit* aTop = NULL, TBool aShort = EFalse) const override;
	virtual string GetRUri(MUnit* aTop = NULL) override;
	virtual TBool RebaseUri(const GUri& aUri, const MUnit* aBase, GUri& aRes) override;
	virtual TBool RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MUnit* aBase, GUri& aRes) override;
	virtual MUnit* GetCommonOwner(MUnit* aElem) override;
	virtual TBool IsRemoved() const override;
	virtual void SetRemoved(TBool aModif) override;
	virtual TBool ChangeAttr(TNodeAttr aAttr, const string& aVal) override;
	virtual MUnit* CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext) override;
	virtual MUnit* GetCompOwning(const string& aParent, MUnit* aElem) override;
	virtual MUnit* GetCompOwning(MUnit* aElem) override;
	virtual const MUnit* GetCompOwning(const MUnit* aElem) const override;
	virtual MUnit* GetCtx() override;
	virtual void SetCtx(MUnit* aOwner) override;
	// Debugging
	virtual TInt GetCapacity() const override;
	virtual TBool IsHeirOf(const string& aParent) const override;
	virtual void DumpIfCache() const override {}
	virtual MUnit* GetNodeS(const char* aUri) override;
	virtual TInt CompsCount() const override;
	virtual MUnit* GetComp(TInt aInd) override;
	virtual void DumpComps(TBool aRecurs = EFalse) const override;
	virtual void DumpContent() const override;
	// Visual client debugging, ref ds_visdbg
	virtual string GetAssociatedData(const string& aUri) const override;
    public:
	// MCompsObserver
	virtual void OnCompDeleting(const MUnit* aComp, TBool aSoft = ETrue, TBool aModif = EFalse) override;
	virtual void OnCompAdding(const MUnit* aComp, TBool aModif = EFalse) override;
	virtual TBool OnCompChanged(const MUnit* aComp, const string& aContName = string(), TBool aModif = EFalse) override;
	virtual TBool OnChanged(const MUnit* aComp) override;
	virtual TBool OnCompRenamed(const MUnit* aComp, const string& aOldName) override;
    public:
	// MOwner
	virtual TBool IsComp(const MUnit* aElem) const override;
	virtual TBool AppendComp(MUnit* aComp, TBool aRt = EFalse) override;
	virtual void RemoveComp(MUnit* aComp) override;
    public:
	// MIfProv
	TIfRange GetIfi(const string& aName, const TICacheRCtx& aCtx) override;
	MIface* getLocalIface(const string& aName, const TICacheRCtx& aCtx) override { return NULL;}
	virtual MIface* GetIfind(const string& aName, const TICacheRCtx& aCtx, TInt aInd) override;
	virtual void UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx) override;
	virtual void UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv) override;
    protected:
	MUnit* NewMunitProxyRequest(const string& aCallSpec);
	const MUnit* NewMunitProxyRequest(const string& aCallSpec) const;
    protected:
	string mName;
};

#endif 
