#ifndef _MIPXPROV_H_
#define _MIPXPROV_H_

#include "mproxy.h"

/*
 * Interface proxy provider interface. Is used for providing interfaces proxies
 * for distributed async agents (DAA) implementation, ref. ds_pp
 */

class DaaProxy;

class MIpxProv: public MIfaceStub
{
    public:
	static const char* Type() { return "MIpxProv";};
	virtual DaaProxy* CreateProxy(const string& aId, MProxyMgr* aMgr, const string& aContext) const = 0;

};

#endif
