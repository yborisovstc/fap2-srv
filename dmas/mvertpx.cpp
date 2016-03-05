
#include <mprov.h>
#include <elem.h>
#include "mvertpx.h"
#include <stdexcept> 


MvertPx::MvertPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MvertPx::~MvertPx()
{
}

bool MvertPx::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Just redirect to mgr
   return mMgr->Request(aContext, aReq, aResp);
}
	
MIface* MvertPx::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	    throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) 
	    throw (runtime_error("Wrong arguments number"));
    return res;
}

TBool MvertPx::Connect(MVert* aPair)
{
    TBool res = EFalse;
    return res;
}

TBool MvertPx::Connect(MEdge* aEdge)
{
    TBool res = EFalse;
    return res;
}

void MvertPx::Disconnect(MVert* aPair)
{
}

void MvertPx::Disconnect(MEdge* aEdge)
{
}

set<MVert*>& MvertPx::Pairs()
{
    __ASSERT(false);
}

Base* MvertPx::EBase()
{
    return NULL;
}

string MvertPx::Uid() const
{
    return GetContext();
}

string MvertPx::Mid() const
{
    return string();
}

void *MvertPx::GetIface(const string& aName)
{
    void *res = NULL;
    if (aName == MVert::Type()) {
	res = (MVert*) this;
    }
    return res;
}

const void *MvertPx::GetIface(const string& aName) const
{
    const void *res = NULL;
    if (aName == MVert::Type()) {
	res = (const MVert*) this;
    }
    return res;
}
