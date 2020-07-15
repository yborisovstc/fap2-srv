#ifndef _DAAPXPROV_H_
#define _DAAPXPROV_H_

#include <elem.h>
#include "mipxprov.h"

/** @brief Base agent for DAA proxies provider, ref. ds_pp_da
*/
class ADaaPxProv: public Elem, public MIpxProv
{
    public:
	static const char* Type() { return "ADaaPxProv";};
	static string PEType();
	ADaaPxProv(const string& aName, MUnit* aMan, MEnv* aEnv);
	ADaaPxProv(MUnit* aMan, MEnv* aEnv);
	virtual ~ADaaPxProv();
    public:
	// From Base
	virtual MIface *DoGetObj(const char *aName);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
	// From MIpxProv
	virtual DaaProxy* CreateProxy(const string& aId, MProxyMgr* aMgr, const string& aContext) const;
};

#endif
