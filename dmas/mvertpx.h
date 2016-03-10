#ifndef _MVERTPX_h_
#define _MVERTPX_h_

#include "daaproxy.h"
#include <mvert.h>
#include <medge.h>
#include <chromo.h>
#include <ifu.h>

/*
 * MVert proxy
 */

class MEdge;

class MvertPx : public DaaProxy, public MVert
{
    public:
	MvertPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MvertPx();
    public:
	virtual void *GetIface(const string& aName);
	virtual const void *GetIface(const string& aName) const;
	// From MVert
	static const char* Type() { return "MVert";};
	virtual TBool Connect(MVert* aPair);
	virtual TBool Connect(MEdge* aEdge);
	virtual void Disconnect(MVert* aPair);
	virtual void Disconnect(MEdge* aEdge);
	virtual set<MVert*>& Pairs();
	virtual void *MVert_DoGetObj(const char *aName);
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
	virtual void *GetIface(const string& aName);
	virtual const void *GetIface(const string& aName) const;
	// From MEdge
	virtual string EdgeName() const;
	virtual string EdgeUri() const;
	virtual TBool ConnectP1(MVert* aPoint);
	virtual TBool ConnectP2(MVert* aPoint);
	virtual void Disconnect(MVert* aPoint);
	virtual void Disconnect();
	virtual MVert* Pair(const MVert* aPoint);
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

#endif 
