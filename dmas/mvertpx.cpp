
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
    string resp;
    string euri = aEdge->EdgeUri();
    TBool rr = mMgr->Request(mContext, "Connect#2,1," + euri, resp);
    if (rr) 
	res = Ifu::ToBool(resp);
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

void *MvertPx::MVert_DoGetObj(const char *aName)
{
    return NewProxyRequest(string("MVert_DoGetObj,1,") + aName, aName);
}

string MvertPx::Uid() const
{
    return GetContext();
}

string MvertPx::Mid() const
{
    return string();
}





// MedgePx


MedgePx::MedgePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MedgePx::~MedgePx()
{
}
	
MIface* MedgePx::Call(const string& aSpec, string& aRes)
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

void *MedgePx::GetIface(const string& aName)
{
    void *res = NULL;
    if (aName == MEdge::Type()) {
	res = (MEdge*) this;
    }
    return res;
}

const void *MedgePx::GetIface(const string& aName) const
{
    const void *res = NULL;
    if (aName == MEdge::Type()) {
	res = (const MEdge*) this;
    }
    return res;
}

string MedgePx::EdgeName() const
{
}

TBool MedgePx::ConnectP1(MVert* aPoint)
{
}

TBool MedgePx::ConnectP2(MVert* aPoint)
{
}

void MedgePx::Disconnect(MVert* aPoint)
{
}

void MedgePx::Disconnect()
{
}

MVert* MedgePx::Pair(const MVert* aPoint)
{
    string resp;
    MVert* point = (MVert*) aPoint;
    MElem* pe = point->GetObj(pe);
    string puri = pe->GetUri(NULL, ETrue);
    return (MVert*) NewProxyRequest(string("Pair,1,") + puri, MVert::Type());
}

MVert* MedgePx::Point1() const
{
}

MVert* MedgePx::Point2() const
{
}

MVert* MedgePx::Ref1() const
{
}

MVert* MedgePx::Ref2() const
{
}

string MedgePx::Uid() const
{
    return GetContext();
}

string MedgePx::Mid() const
{
    return string();
}

string MedgePx::EdgeUri() const
{
    string resp;
    TBool rr = mMgr->Request(mContext, "EdgeUri", resp);
    return resp;
}


