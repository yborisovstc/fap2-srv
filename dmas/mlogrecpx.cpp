#include <mprov.h>
#include "mlogrecpx.h"
#include <stdexcept> 


MlogrecPx::MlogrecPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MlogrecPx::~MlogrecPx()
{
}

MIface* MlogrecPx::GetIface(const string& aName)
{
    return NULL;
}

const MIface* MlogrecPx::GetIface(const string& aName) const
{
    return NULL;
}

string MlogrecPx::GetUid() const
{
    __ASSERT(false);
    return Uid();
}

void MlogrecPx::WriteFormat(const char* aFmt,...)
{
    __ASSERT(false);
}

void MlogrecPx::Write(TLogRecCtg aCtg, const MUnit* aNode, const char* aFmt,...)
{
    __ASSERT(false);
}

void MlogrecPx::Write(TLogRecCtg aCtg, MUnit* aNode, const ChromoNode& aMut, const char* aFmt,...)
{
    __ASSERT(false);
}

void MlogrecPx::Flush()
{
    __ASSERT(false);
}

void MlogrecPx::SetContextMutId(TInt aMutId)
{
    __ASSERT(false);
}

TBool MlogrecPx::AddLogObserver(MLogObserver* aObs)
{
    __ASSERT(false);
}

void MlogrecPx::RemoveLogObserver(MLogObserver* aObs)
{
    __ASSERT(false);
}

TInt MlogrecPx::GetStat(TLogRecCtg aCtg) const
{
    __ASSERT(false);
}

void MlogrecPx::Write(const TLog& aRec)
{
    string resp;
    string req = Ifu::CombineIcSpec("Write", "1");
    Ifu::AddIcSpecArg(req, aRec);
    Request(req, resp);
}

string MlogrecPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MlogrecPx::Mid() const
{
    return string();
}

MIface* MlogrecPx::Call(const string& aSpec, string& aRes)
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
    Request(aSpec, aRes);
    return res;
}
