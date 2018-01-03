
#include <mprov.h>
#include "mdespx.h"
#include <stdexcept> 


MDesObserverPx::MDesObserverPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MDesObserverPx::~MDesObserverPx()
{
}
	
MIface* MDesObserverPx::Call(const string& aSpec, string& aRes)
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

MIface* MDesObserverPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MDesObserver::Type()) {
	res = (MDesObserver*) this;
    }
    return res;
}

const MIface* MDesObserverPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MDesObserver::Type()) {
	res = (const MDesObserver*) this;
    }
    return res;
}

string MDesObserverPx::Mid() const
{
    return mMgr->Oid();
}

string MDesObserverPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

void MDesObserverPx::OnUpdated()
{
    Rpcv(__func__);
}

void MDesObserverPx::OnActivated()
{
    Rpcv(__func__);
}




MDesSyncablePx::MDesSyncablePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MDesSyncablePx::~MDesSyncablePx()
{
}
	
MIface* MDesSyncablePx::Call(const string& aSpec, string& aRes)
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

MIface* MDesSyncablePx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MDesSyncablePx::Type()) {
	res = (MDesSyncablePx*) this;
    }
    return res;
}

const MIface* MDesSyncablePx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MDesSyncablePx::Type()) {
	res = (const MDesSyncablePx*) this;
    }
    return res;
}

string MDesSyncablePx::Mid() const
{
    return mMgr->Oid();
}

string MDesSyncablePx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

void MDesSyncablePx::Update()
{
    Rpcv(__func__);
}

void MDesSyncablePx::Confirm()
{
    Rpcv(__func__);
}

TBool MDesSyncablePx::IsUpdated()
{
    return Rpc<TBool>(__func__);
}

void MDesSyncablePx::SetUpdated()
{
    Rpcv(__func__);
}

void MDesSyncablePx::ResetUpdated()
{
    Rpcv(__func__);
}

TBool MDesSyncablePx::IsActive()
{
    return Rpc<TBool>(__func__);
}

void MDesSyncablePx::SetActive()
{
    Rpcv(__func__);
}

void MDesSyncablePx::ResetActive()
{
    Rpcv(__func__);
}


