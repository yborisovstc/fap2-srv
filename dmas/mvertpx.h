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
	virtual TBool Connect(MVert* aPair);
	virtual void Disconnect(MVert* aPair);
	virtual TInt PairsCount() const;
	virtual MVert* GetPair(TInt aInd) const;
	virtual TBool IsPair(const MVert* aPair) const;
	virtual MIface* MVert_DoGetObj(const char *aName);
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
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
	virtual string EdgeName() const;
	virtual string EdgeUri() const;
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Point1() const;
	virtual MVert* Point2() const;
	virtual MVert* Ref1() const;
	virtual MVert* Ref2() const;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
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
	virtual TBool IsCompatible(MElem* aPair, TBool aExt = EFalse);
	virtual MElem* GetExtd();
	virtual TDir GetDir() const;
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

#endif 
