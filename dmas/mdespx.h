#ifndef _MDESPX_h_
#define _MDESPX_h_

#include "daaproxy.h"
#include <mdes.h>


// Observer. Upper layer is observer of components 
class MDesObserverPx : public DaaProxy, public MDesObserver
{
    public:
	MDesObserverPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MDesObserverPx();
    public:
	// From MProxy
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
};

class MDesSyncablePx : public DaaProxy, public MDesSyncable
{
    public:
	MDesSyncablePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MDesSyncablePx();
    public:
	// From MProxy
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
	// From MDesSyncable
	virtual void Update();
	virtual void Confirm();
	virtual TBool IsUpdated();
	virtual void SetUpdated();
	virtual void ResetUpdated();
	virtual TBool IsActive();
	virtual void SetActive();
	virtual void ResetActive();
};

#endif
