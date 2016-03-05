#ifndef _MVERTPX_h_
#define _MVERTPX_h_

#include "daaproxy.h"
#include <mvert.h>
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
	virtual Base* EBase();
	// From MIface	
	virtual string Uid() const;
	virtual string Mid() const;
    public:
	// From MProxyMgr
	virtual bool Request(const string& aContext, const string& aReq, string& aResp);
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
};

#endif 
