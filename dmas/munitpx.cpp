
#include <mprov.h>
#include "munitpx.h"
#include "mipxprov.h"
#include <unit.h>
#include <stdexcept> 
#include "../server/requests.h"


MunitPx::IfIter::IfIter(MunitPx* aHost, const string& aIfName, const TICacheRCtx& aReq, int aInd): mHost(aHost), mIfName(aIfName), mReq(aReq), mInd(aInd)
{}

MunitPx::IfIter::IfIter(const IfIter& aIt): mHost(aIt.mHost), mIfName(aIt.mIfName), mInd(aIt.mInd) {}

MUnit::MIfIter& MunitPx::IfIter::operator=(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    mHost = it.mHost; 
    mInd = it.mInd;
    mIfName = it.mIfName;
    mReq = it.mReq;
    return *this;
}

MUnit::MIfIter* MunitPx::IfIter::Clone() const
{
    return new IfIter(*this);
}

MUnit::MIfIter& MunitPx::IfIter::operator++()
{
    mInd++;
    return *this;
}

TBool MunitPx::IfIter::operator==(const MIfIter& aIt)
{
    const IfIter& it = (const IfIter&) aIt;
    TBool res = (mHost == it.mHost && mInd == it.mInd && mIfName == it.mIfName && mReq == it.mReq);
    return res;
}

MIface*  MunitPx::IfIter::operator*()
{
    MIface* res = mHost->GetIfind(mIfName, mReq, mInd);
    return res;
}



MunitPx::MunitPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
}

MunitPx::~MunitPx()
{
}

void MunitPx::Delete()
{
    string resp;
    string req("Delete,1");
    TBool rr = mMgr->Request(mContext, req, resp);
    if (!rr) {
	Logger()->Write(EErr, NULL, "Proxy [%s]: [%s] request failed: %s",
		Uid().c_str(), req.c_str(), resp.c_str());
    }
}

bool MunitPx::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Just redirect to mgr
   return mMgr->Request(aContext, aReq, aResp);
}

MIface* MunitPx::Call(const string& aSpec, string& aRes)
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

MUnit* MunitPx::NewMunitProxyRequest(const string& aCallSpec)
{
    return (MUnit*) NewProxyRequest(aCallSpec, MUnit::Type());
}

const MUnit* MunitPx::NewMunitProxyRequest(const string& aCallSpec) const
{
    return (const MUnit*) NewProxyRequest(aCallSpec, MUnit::Type());
}

string MunitPx::EType(TBool aShort) const
{
    return Rpc<string, TBool>(__func__, aShort);
}

const string& MunitPx::Name() const
{
    string resp;
    TBool res = mMgr->Request(mContext, "Name", resp);
    if (res) 
	((MunitPx*) this)->mName = resp;
    return mName;
}

MUnit* MunitPx::GetMan()
{
    return RpcPx<MUnit>(__func__);
}

const MUnit* MunitPx::GetMan() const
{
    return RpcPx<MUnit>(__func__);
}

// Ignoring. Only one way relation is allowed for the proxy
// TODO [YB] To check if assert makes sense here
void MunitPx::SetMan(MUnit* aMan)
{
}

MUnit* MunitPx::GetNode(const string& aUri, TBool aInclRm) 
{
    return RpcPx<MUnit>(__func__, aUri, aInclRm);
}

MUnit* MunitPx::GetNode(const GUri& aUri, TBool aInclRm)
{
    return GetNode(aUri.toString(ETrue), aInclRm);
}

// TODO [YB] Implicit reduction of args is used here (aUri is not sent). To check for correctness.
MUnit* MunitPx::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, TBool aInclRm)
{ 
    MUnit* res = NULL;
    string suri = aUri.toString(aPathBase);
    string cspec = "GetNode#2,1," + suri + "," + Ifu::FromBool(aAnywhere) + "," + Ifu::FromBool(aInclRm);
    res = NewMunitProxyRequest(cspec);
    return res;
}

MUnit* MunitPx::GetRoot() const
{
    return RpcPx<MUnit>(__func__);
}

TInt MunitPx::GetContCount(const string& aName) const
{
    return Rpc<TInt, const string&>(__func__, aName);
}

TBool MunitPx::ContentExists(const string& aName) const
{
    return Rpc<TBool, const string&>(__func__, aName);
}
 
string MunitPx::GetContComp(const string& aOwnerName, TInt aInd) const
{
    return Rpc<string, const string&, TInt>(__func__, aOwnerName, aInd);
}

string MunitPx::GetContent(const string& aName, TBool aFull) const
{
    return Rpc<string, const string&, TBool>(__func__, aName, aFull);
}

TBool MunitPx::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName) {return false;}
 
void MunitPx::GetUri(GUri& aUri, MUnit* aTop) const
{
    string uri = GetUri(aTop, ETrue);
    GUri guri(uri);
    aUri.Prepend(guri);
}

void MunitPx::GetRUri(GUri& aUri, MUnit* aTop)
{
    string res = GetRUri(aTop);
    aUri = GUri(res);
}

string MunitPx::GetUri(MUnit* aTop, TBool aShort) const
{
    return Rpc<string, MUnit*>(__func__, aTop);
}

string MunitPx::GetRUri(MUnit* aTop)
{
    return Rpc<string, MUnit*>(__func__, aTop);
}

TBool MunitPx::RebaseUri(const GUri& aUri, const MUnit* aBase, GUri& aRes) {return false;}

TBool MunitPx::RebaseUri(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere, const MUnit* aBase, GUri& aRes) {return false;}

MUnit* MunitPx::GetCommonOwner(MUnit* aElem)
{
    __ASSERT(false);
    return NULL;
}

TBool MunitPx::IsRemoved() const
{
    return Rpc<TBool>(__func__);
}

void MunitPx::SetRemoved(TBool aModif)
{
    __ASSERT(false);
}

TBool MunitPx::ChangeAttr(TNodeAttr aAttr, const string& aVal)
{
    __ASSERT(false);
    return false;
}
 
MUnit* MunitPx::GetCompOwning(const string& aParent, MUnit* aElem)
{
    __ASSERT(false);
    return NULL;
}

MUnit* MunitPx::GetCompOwning(MUnit* aElem)
{
    return RpcPx<MUnit, MUnit*>(__func__, aElem);
}

const MUnit* MunitPx::GetCompOwning(const MUnit* aElem) const
{
    return RpcPxC<MUnit, const MUnit*>(__func__, aElem);
}

// TODO [YB] To implement
TInt MunitPx::GetCapacity() const
{
    return 0;
}

TBool MunitPx::IsHeirOf(const string& aParent) const
{
    return Rpc<TBool, string>(__func__, aParent);
}

TInt MunitPx::CompsCount() const
{
    return Rpc<TInt>(__func__);
}

MUnit* MunitPx::GetComp(const string& aParent, const string& aName) const
{
    // TODO [YB] To implement auto-generation of method signature (to avoid suffix #2)
    return RpcPx<MUnit>("GetComp#2", aParent, aName);
}

MUnit*  MunitPx::CreateHeir(const string& aName, MUnit* aMan, MUnit* aContext)
{
    return RpcPx<MUnit, const string&, MUnit*, MUnit*>(__func__, aName, aMan, aContext);
}

void  MunitPx::OnCompDeleting(const MUnit* aComp, TBool aSoft, TBool aModif)
{
    Rpcv<const MUnit*, TBool, TBool>(__func__, aComp, aSoft, aModif);
}

void  MunitPx::OnCompAdding(const MUnit* aComp, TBool aModif)
{
    Rpcv<const MUnit*, TBool>(__func__, aComp, aModif);
}

// TODO [YB] aModif isn't used anymore, to cleanup
TBool  MunitPx::OnCompChanged(const MUnit* aComp, const string& aContName, TBool aModif)
{
    return Rpc<TBool, const MUnit*, string>(__func__, aComp, aContName);
}

TBool  MunitPx::OnChanged(const MUnit* aComp)
{
    return Rpc<TBool, const MUnit*>(__func__, aComp);
}

TBool MunitPx::OnCompRenamed(const MUnit* aComp, const string& aOldName)
{
    return Rpc<TBool, const MUnit*, string>(__func__, aComp, aOldName);
}

TBool  MunitPx::IsComp(const MUnit* aElem) const
{
    return Rpc<TBool, const MUnit*>(__func__, aElem);
}

TBool  MunitPx::AppendComp(MUnit* aComp, TBool aRt)
{
    return Rpc<TBool, MUnit*, TBool>(__func__, aComp, aRt);
}

void MunitPx::RemoveComp(MUnit* aComp)
{
    Rpcv<MUnit*>(__func__, aComp);
}

// MIfProv


MUnit::TIfRange  MunitPx::GetIfi(const string& aName, const TICacheRCtx& aCtx)
{
    TInt ifcnt = Rpc<TInt, const string&, const TICacheRCtx&>(__func__, aName, aCtx);
    IfIter beg(this, aName, aCtx, 0);
    IfIter end(this, aName, aCtx, ifcnt);
    return TIfRange(beg, end);
}

// Debugging

MUnit* MunitPx::GetNodeS(const char* aUri)
{
    __ASSERT(false);
};

MUnit* MunitPx::GetComp(TInt aInd)
{
    return RpcPx<MUnit>(__func__, aInd);
};

MIface *MunitPx::DoGetObj(const char *aName)
{
return RpcPxN(__func__, aName, aName);
}

TBool MunitPx::IsProvided() const
{
    return Rpc<TBool>(__func__);
}

string MunitPx::Mid() const
{
    return mMgr->Oid();
}

string MunitPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}


MIface* MunitPx::GetIface(const string& aName)
{
    MIface* res = NULL;
    if (aName == MUnit::Type()) {
	res = (MUnit*) this;
    }
    return res;
}

const MIface* MunitPx::GetIface(const string& aName) const
{
    const MIface* res = NULL;
    if (aName == MUnit::Type()) {
	res = (const MUnit*) this;
    }
    return res;
}

MIface* MunitPx::GetIfind(const string& aName, const TICacheRCtx& aCtx, TInt aInd)
{
    return RpcPxN(__func__, aName, aName, aCtx, aInd);
}

void MunitPx::UnregIfReq(const string& aIfName, const TICacheRCtx& aCtx)
{
    Rpcv<const string&, const TICacheRCtx&>(__func__, aIfName, aCtx);
}

void MunitPx::UnregIfProv(const string& aIfName, const TICacheRCtx& aCtx, MUnit* aProv)
{
    Rpcv<const string&, const TICacheRCtx&, MUnit*>(__func__, aIfName, aCtx, aProv);
}

void MunitPx::DumpContent() const
{
    __ASSERT(false);
}

TBool MunitPx::ContValueExists(const string& aName) const
{
    return Rpc<TBool, const string&>(__func__, aName);
}

TBool MunitPx::IsContOfCategory(const string& aName, const string& aCategory) const
{
    return Rpc<TBool, const string&, const string&>(__func__, aName, aCategory);
}

void MunitPx::DumpComps(TBool aRecurs) const
{
    __ASSERT(EFalse);
}

string MunitPx::GetAssociatedData(const string& aUri) const
{
    __ASSERT(EFalse);
}

MUnit* MunitPx::GetCtx()
{
    return NewMunitProxyRequest("GetCtx");
}

void MunitPx::SetCtx(MUnit* aOwner)
{
    __ASSERT(EFalse);
}

