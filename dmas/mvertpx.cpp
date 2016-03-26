
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
    string resp;
    MElem* epair = aPair->MVert::GetObj(epair);
    string puri = epair->GetUri(NULL, ETrue);
    TBool rr = mMgr->Request(mContext, "Connect,1," + puri, resp);
    if (rr) 
	res = Ifu::ToBool(resp);
    return res;
}

void MvertPx::Disconnect(MVert* aPair)
{
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

TInt MvertPx::PairsCount() const
{
    assert(false);
}

MVert* MvertPx::GetPair(TInt aInd) const
{
    assert(false);
}

TBool MvertPx::IsPair(const MVert* aPair) const
{
    TBool res = EFalse;
    MElem* epair = ((MVert*) aPair)->MVert::GetObj(epair);
    string pair_uri = epair->GetUri(NULL, ETrue);
    string resp;
    TBool rr = mMgr->Request(mContext, string("IsPair,1,") + pair_uri, resp);
    if (rr) 
	res = Ifu::ToBool(resp);
    return res;

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

/*
MVert* MedgePx::Pair(const MVert* aPoint)
{
    string resp;
    MVert* point = (MVert*) aPoint;
    MElem* pe = point->GetObj(pe);
    string puri = pe->GetUri(NULL, ETrue);
    return (MVert*) NewProxyRequest(string("Pair,1,") + puri, MVert::Type());
}
*/

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



MCompatCheckerPx::MCompatCheckerPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MCompatCheckerPx::~MCompatCheckerPx()
{
}
	
MIface* MCompatCheckerPx::Call(const string& aSpec, string& aRes)
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
    if (name == "GetExtd") {
	res = (MIface*) NewProxyRequest(aSpec, MElem::Type());
    } else if (name == "GetDir") {
	TBool rr = mMgr->Request(mContext, aSpec, aRes);
    } else if (name == "IsCompatible") {
	TBool rr = mMgr->Request(mContext, aSpec, aRes);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }

    return res;
}

string MCompatCheckerPx::Uid() const
{
    return GetContext();
}

string MCompatCheckerPx::Mid() const
{
    return string();
}

TBool MCompatCheckerPx::IsCompatible(MElem* aPair, TBool aExt)
{
    TBool res = EFalse;
    string uri = aPair->GetUri(NULL, ETrue);
    string ext = Ifu::FromBool(aExt);
    string resp;
    TBool rr = mMgr->Request(mContext, string("IsCompatible,1,") + uri + "," + ext, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return  res;
}

MElem* MCompatCheckerPx::GetExtd()
{
    return (MElem*) NewProxyRequest("GetExtd,1", MElem::Type());
}

MCompatChecker::TDir MCompatCheckerPx::GetDir() const
{
    TDir res = ERegular;
    string resp;
    TBool rr = mMgr->Request(mContext, "GetDir,1", resp);
    res = (TDir) Ifu::ToInt(resp);
    return res;
}

void *MCompatCheckerPx::GetIface(const string& aName)
{
    void *res = NULL;
    if (aName == MCompatChecker::Type()) {
	res = (MCompatChecker*) this;
    }
    return res;
}

const void *MCompatCheckerPx::GetIface(const string& aName) const
{
    const void *res = NULL;
    if (aName == MCompatChecker::Type()) {
	res = (const MCompatChecker*) this;
    }
    return res;
}





MPropPx::MPropPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MPropPx::~MPropPx()
{
}
	
MIface* MPropPx::Call(const string& aSpec, string& aRes)
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
    if (name == "Value") {
	TBool rr = mMgr->Request(mContext, aSpec, aRes);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }

    return res;
}

string MPropPx::Uid() const
{
    return GetContext();
}

string MPropPx::Mid() const
{
    return string();
}

const string& MPropPx::Value() const
{
    MPropPx* self = (MPropPx*) this;
    TBool rr = mMgr->Request(mContext, "Value,1", self->mValue);
    return mValue;
}

void *MPropPx::GetIface(const string& aName)
{
    void *res = NULL;
    if (aName == MProp::Type()) {
	res = (MProp*) this;
    }
    return res;
}

const void *MPropPx::GetIface(const string& aName) const
{
    const void *res = NULL;
    if (aName == MProp::Type()) {
	res = (const MProp*) this;
    }
    return res;
}






