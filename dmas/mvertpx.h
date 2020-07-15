#ifndef _MVERTPX_h_
#define _MVERTPX_h_

#include "daaproxy.h"
#include <mvert.h>
#include <medge.h>
#include <chromo.h>
#include <ifu.h>
#include <mprop.h>

/*
 * MVert proxy
 */

class MEdge;
class MElem;

class MvertPx : public DaaProxy, public MVert
{
    public:
	MvertPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MvertPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MVert
	static const char* Type() { return "MVert";};
	virtual TBool Connect(MVert* aPair) override;
	virtual void Disconnect(MVert* aPair) override;
	virtual TInt PairsCount() const override;
	virtual MVert* GetPair(TInt aInd) const override;
	virtual TBool IsPair(const MVert* aPair) const override;
	virtual TBool IsLinked(const MVert* aPair, TBool aDirect = EFalse) const override;
	virtual MIface* MVert_DoGetObj(const char *aName) override;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes) override;
	virtual string Uid() const override;
	virtual string Mid() const override;
	virtual string MVert_Mid() const override;
};

class MedgePx : public DaaProxy, public MEdge
{
    public:
	MedgePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MedgePx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MEdge
	virtual TBool ConnectP1(MVert* aPoint) override;
	virtual TBool ConnectP2(MVert* aPoint) override;
	virtual void Disconnect(MVert* aPoint) override;
	virtual void Disconnect() override;
	virtual MVert* Point1() const override;
	virtual MVert* Point2() const override;
	virtual MVert* Ref1() const override;
	virtual MVert* Ref2() const override;
	virtual void SetPoint1(const string& aRef) override;
	virtual void SetPoint2(const string& aRef) override;
	// From MEdge.MIface	
	virtual MIface* MEdge_Call(const string& aSpec, string& aRes) override;
	virtual string Uid() const override;
	virtual string MEdge_Mid() const override;
};

class MCompatCheckerPx : public DaaProxy, public MCompatChecker
{
    public:
	MCompatCheckerPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MCompatCheckerPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MCompatChecker
	virtual TBool IsCompatible(MUnit* aPair, TBool aExt = EFalse) override;
	virtual MUnit* GetExtd() override;
	virtual TDir GetDir() const override;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
};

class MPropPx : public DaaProxy, public MProp
{
    public:
	MPropPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MPropPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MProp
	virtual const string& Value() const;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
    protected:
	string mValue;
};

class MConnPointPx: public DaaProxy, public MConnPoint
{
    public:
	MConnPointPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MConnPointPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MConnPoint
	virtual TBool IsProvided(const string& aIfName) const;
	virtual TBool IsRequired(const string& aIfName) const;
	virtual string Provided() const;
	virtual string Required() const;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
};

class MSocketPx: public DaaProxy, public MSocket
{
    public:
	MSocketPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MSocketPx();
    public:
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MSocket
	virtual TInt PinsCount() const override;
	virtual MUnit* GetPin(TInt aInd) override;
	MUnit* GetPin(const TICacheRCtx& aCtx) override { return NULL;}
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
};

#endif 
