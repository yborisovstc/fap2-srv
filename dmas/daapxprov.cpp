
#include <plat.h>
#include "daapxprov.h"
#include "melempx.h"
#include "mvertpx.h"


string ADaaPxProv::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADaaPxProv::ADaaPxProv(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv)
{
    SetParent(Type());
}

ADaaPxProv::ADaaPxProv(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv)
{
    SetParent(Elem::PEType());
}

ADaaPxProv::~ADaaPxProv() {
}

void *ADaaPxProv::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MIpxProv::Type()) == 0) {
	res = (MIpxProv*) this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

void ADaaPxProv::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    Logger()->Write(EErr, this, "Mutation of base proxy provider is not allowed");
}

DaaProxy* ADaaPxProv::CreateProxy(const string& aId, MProxyMgr* aMgr, const string& aContext) const
{
    DaaProxy* res = NULL;
    if (aId == MElem::Type()) {
	res = new MelemPx(iEnv, aMgr, aContext);
    } else if (aId == MVert::Type()) {
	res = new MvertPx(iEnv, aMgr, aContext);
    } else if (aId == MedgePx::Type()) {
	res = new MedgePx(iEnv, aMgr, aContext);
    } else if (aId == MCompatCheckerPx::Type()) {
	res = new MCompatCheckerPx(iEnv, aMgr, aContext);
    } else if (aId == MProp::Type()) {
	res = new MPropPx(iEnv, aMgr, aContext);
    } else if (aId == MConnPoint::Type()) {
	res = new MConnPointPx(iEnv, aMgr, aContext);
    } else if (aId == MSocket::Type()) {
	res = new MSocketPx(iEnv, aMgr, aContext);
    } else {
	__ASSERT(EFalse);
    }
    return res;
}
