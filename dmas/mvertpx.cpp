
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
    if (!args_ok) {
	throw (runtime_error("Wrong arguments number"));
    } else {
	mMgr->Request(mContext, aSpec, aRes);
    }
    return res;
}

TBool MvertPx::Connect(MVert* aPair)
{
    return Rpc<TBool, MVert*>(__func__, aPair);
}

void MvertPx::Disconnect(MVert* aPair)
{
    return Rpcv<MVert*>(__func__, aPair);
}

MIface* MvertPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MVert::Type()) {
	res = (MVert*) this;
    }
    return res;
}

const MIface* MvertPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MVert::Type()) {
	res = (const MVert*) this;
    }
    return res;
}

MIface* MvertPx::MVert_DoGetObj(const char *aName)
{
    return NewProxyRequest(string("MVert_DoGetObj,1,") + aName, aName);
}

string MvertPx::Mid() const
{
    return mMgr->Oid();
}

string MvertPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MvertPx::MVert_Mid() const
{
    return mMgr->Oid();
}

TInt MvertPx::PairsCount() const
{
    TInt res = 0;
    string resp;
    string req = Ifu::CombineIcSpec("PairsCount", "1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToInt(resp);
    } else {
	Logger()->Write(EErr, NULL, "Proxy [%s]: request [%s] failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    } 
    return res;
}

MVert* MvertPx::GetPair(TInt aInd) const
{
    assert(false);
}

TBool MvertPx::IsPair(const MVert* aPair) const
{
    return Rpc<TBool, const MVert*>(__func__, aPair);
}

TBool MvertPx::IsLinked(const MVert* aPair, TBool aDirect) const
{
    return Rpc<TBool, const MVert*, TBool>(__func__, aPair, aDirect);
}



// MedgePx


MedgePx::MedgePx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MedgePx::~MedgePx()
{
}
	
MIface* MedgePx::MEdge_Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) {
	throw (runtime_error("Wrong arguments number"));
    } else {
	mMgr->Request(mContext, aSpec, aRes);
    }
    return res;
}

MIface* MedgePx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MEdge::Type()) {
	res = (MEdge*) this;
    }
    return res;
}

const MIface* MedgePx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MEdge::Type()) {
	res = (const MEdge*) this;
    }
    return res;
}

TBool MedgePx::ConnectP1(MVert* aPoint)
{
    return Rpc<TBool, MVert*>(__func__, aPoint);
}

TBool MedgePx::ConnectP2(MVert* aPoint)
{
    return Rpc<TBool, MVert*>(__func__, aPoint);
}

void MedgePx::Disconnect(MVert* aPoint)
{
    __ASSERT(EFalse); // To implement
}

void MedgePx::Disconnect()
{
    __ASSERT(EFalse); // To implement
}

MVert* MedgePx::Point1() const
{
    return (MVert*) NewProxyRequest("Point1,0", MVert::Type());
}

MVert* MedgePx::Point2() const
{
    return (MVert*) NewProxyRequest("Point2,0", MVert::Type());
}

MVert* MedgePx::Ref1() const
{
    return (MVert*) NewProxyRequest("Ref1,0", MVert::Type());
}

MVert* MedgePx::Ref2() const
{
    return (MVert*) NewProxyRequest("Ref2,0", MVert::Type());
}

string MedgePx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MedgePx::MEdge_Mid() const
{
    return mMgr->Oid();
}

void MedgePx::SetPoint1(const string& aRef)
{
    __ASSERT(EFalse); // To implement
}

void MedgePx::SetPoint2(const string& aRef)
{
    __ASSERT(EFalse); // To implement
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
    if (!args_ok) {
	throw (runtime_error("Wrong arguments number"));
    } else {
	mMgr->Request(mContext, aSpec, aRes);
    }
    return res;
}

string MCompatCheckerPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MCompatCheckerPx::Mid() const
{
    return mMgr->Oid();
}

TBool MCompatCheckerPx::IsCompatible(MUnit* aPair, TBool aExt)
{
    return Rpc<TBool, MUnit*, TBool>(__func__, aPair, aExt);
}

MUnit* MCompatCheckerPx::GetExtd()
{
    return RpcPx<MUnit>(__func__);
}

MCompatChecker::TDir MCompatCheckerPx::GetDir() const
{
    return Rpc<MCompatChecker::TDir>(__func__);
    /*
       TDir res = ERegular;
    string resp;
    TBool rr = mMgr->Request(mContext, "GetDir,1", resp);
    if (rr) {
	res = (TDir) Ifu::ToInt(resp);
    }
    return res;
    */
}

MIface* MCompatCheckerPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MCompatChecker::Type()) {
	res = (MCompatChecker*) this;
    }
    return res;
}

const MIface* MCompatCheckerPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
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
	mMgr->Request(mContext, aSpec, aRes);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }

    return res;
}

string MPropPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MPropPx::Mid() const
{
    return mMgr->Oid();
}

const string& MPropPx::Value() const
{
    MPropPx* self = (MPropPx*) this;
    string resp;
    TBool rr = mMgr->Request(mContext, "Value,1", resp);
    if (rr) {
	self->mValue = resp;
    }
    return mValue;
}

MIface* MPropPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MProp::Type()) {
	res = (MProp*) this;
    }
    return res;
}

const MIface* MPropPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MProp::Type()) {
	res = (const MProp*) this;
    }
    return res;
}



MConnPointPx::MConnPointPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MConnPointPx::~MConnPointPx()
{
}
	
MIface* MConnPointPx::Call(const string& aSpec, string& aRes)
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
	mMgr->Request(mContext, aSpec, aRes);
    } else {
	throw (runtime_error("Unhandled method: " + name));
    }

    return res;
}

string MConnPointPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MConnPointPx::Mid() const
{
    return mMgr->Oid();
}

MIface* MConnPointPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MConnPoint::Type()) {
	res = (MConnPoint*) this;
    }
    return res;
}

const MIface* MConnPointPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MConnPoint::Type()) {
	res = (const MConnPoint*) this;
    }
    return res;
}


TBool MConnPointPx::IsProvided(const string& aIfName) const
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("IsProvided", "1");
    Ifu::AddIcSpecArg(req, aIfName);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

TBool MConnPointPx::IsRequired(const string& aIfName) const
{
    TBool res = EFalse;
    string resp;
    string req = Ifu::CombineIcSpec("IsRequired", "1");
    Ifu::AddIcSpecArg(req, aIfName);
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToBool(resp);
    }
    return res;
}

string MConnPointPx::Provided() const
{
    string res;
    string resp;
    string req = Ifu::CombineIcSpec("Provided", "1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = resp;
    }
    return res;
}

string MConnPointPx::Required() const
{
    string res;
    string resp;
    string req = Ifu::CombineIcSpec("Required", "1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = resp;
    }
    return res;
}




MSocketPx::MSocketPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MSocketPx::~MSocketPx()
{
}
	
MIface* MSocketPx::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) 
	throw (runtime_error("Wrong method name"));
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) {
	throw (runtime_error("Wrong arguments number"));
    } else {
	mMgr->Request(mContext, aSpec, aRes);
    }
    return res;
}

string MSocketPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

string MSocketPx::Mid() const
{
    return mMgr->Oid();
}

MIface* MSocketPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MSocket::Type()) {
	res = (MSocket*) this;
    }
    return res;
}

const MIface* MSocketPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MSocket::Type()) {
	res = (const MSocket*) this;
    }
    return res;
}

TInt MSocketPx::PinsCount() const
{
    TInt res = 0;
    string resp;
    string req = Ifu::CombineIcSpec("PinsCount", "1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (rr) {
	res = Ifu::ToInt(resp);
    }
    return res;
}

MUnit* MSocketPx::GetPin(TInt aInd)
{
    return RpcPx<MUnit, TInt>(__func__, aInd);
}





