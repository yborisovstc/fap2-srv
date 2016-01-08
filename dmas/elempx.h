#ifndef _melempx_h_
#define _melempx_h_

#include <melem.h>
#include "mmuta.h"
#include <elem.h>
#include "mproxy.h"

/*
 * Native hier node front proxy
 */
class ElemFp: public Elem, public MProxy
{
    public:
	ElemFp(MProxyMgr& aProxyMgr, Elem* aMan = NULL, MEnv* aEnv = NULL);
	virtual ~ElemFp();
    protected:
	MProxyMgr& mProxyMgr;
};

#endif
