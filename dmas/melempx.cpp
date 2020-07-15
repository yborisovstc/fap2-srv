
#include <mprov.h>
#include <elem.h>
#include "melempx.h"
#include "mvertpx.h"
#include "mipxprov.h"
#include <stdexcept> 
#include "../server/requests.h"



MelemPx::MelemPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext): DaaProxy(aEnv, aMgr, aContext)
{
    mChromo = Provider()->CreateChromo();
    mChromo->Init(ENt_Node);
}

MelemPx::~MelemPx()
{
    if (mChromo) {
	delete mChromo;
    }
}

bool MelemPx::Request(const string& aContext, const string& aReq, string& aResp)
{
    // Just redirect to mgr
   return mMgr->Request(aContext, aReq, aResp);
}

MIface* MelemPx::MElem_Call(const string& aSpec, string& aRes)
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

MElem* MelemPx::NewMElemProxyRequest(const string& aCallSpec)
{
    return (MElem*) NewProxyRequest(aCallSpec, MElem::Type());
}

const MElem* MelemPx::NewMElemProxyRequest(const string& aCallSpec) const
{
    return (const MElem*) NewProxyRequest(aCallSpec, MElem::Type());
}

void MelemPx::GetCRoot(TMut& aMut) const
{
    aMut = Rpc<string>(__func__);
}

MUnit* MelemPx::GetInhRoot() const
{
    __ASSERT(false);
    return NULL;
}

void MelemPx::Mutate(TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    Rpcv<TBool, TBool, TBool, const MutCtx&>("Mutate#2", aRunTimeOnly, aCheckSafety, aTrialMode, aCtx);
}

void MelemPx::Mutate(const ChromoNode& aMutsRoot, TBool aRunTimeOnly, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
}

MUnit* MelemPx::GetUnit()
{
    return RpcPx<MUnit>(__func__);
}

MUnit* MelemPx::GetAowner()
{
    return RpcPx<MUnit>(__func__);
}

const MUnit* MelemPx::GetAowner() const
{
    return RpcPx<const MUnit>(__func__);
}

MUnit* MelemPx::GetAcompOwning(MUnit* aComp)
{
    __ASSERT(false);
    return NULL;
}

MUnit* MelemPx::GetUpperAowner()
{
    __ASSERT(false);
    return NULL;
}

MUnit* MelemPx::GetAttachedMgr()
{
    __ASSERT(false);
    return NULL;
}

const MUnit* MelemPx::GetAttachedMgr() const
{
    __ASSERT(false);
    return NULL;
}

TBool MelemPx::IsAownerOf(const MUnit* aElem) const
{
    __ASSERT(false);
    return false;
}

TInt MelemPx::GetCompLrank(const MUnit* aComp) const
{
    return Rpc<TInt, const MUnit*>(__func__, aComp);
}

TBool MelemPx::IsInheritedComp(const MUnit* aNode) const
{
    __ASSERT(false);
    return false;
}

MUnit* MelemPx::GetCompAowner(const MUnit* aComp)
{
    return RpcPx<MUnit, const MUnit*>(__func__, aComp);
}

const MUnit* MelemPx::GetCompAowner(const MUnit* aComp) const
{
    return RpcPxC<MUnit, const MUnit*>(__func__, aComp);
}

TBool MelemPx::HasInherDeps(const MUnit* aScope) const
{
    __ASSERT(false);
    return false;
}

TBool MelemPx::IsCompAttached(const MUnit* aComp) const
{
    return Rpc<TBool, const MUnit*>(__func__, aComp);
}

void MelemPx::DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
}

TBool  MelemPx::DoMutChangeCont(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
    return false;
}

void MelemPx::ChangeAttr(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
}

MUnit*  MelemPx::AddElem(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
    return NULL;
}

TBool  MelemPx::RmNode(const ChromoNode& aSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MutCtx& aCtx)
{
    __ASSERT(false);
    return false;
}

const MChromo&  MelemPx::Chromos() const
{
    return *mChromo;
}

MChromo&  MelemPx::Chromos()
{
    return *mChromo;
}

void  MelemPx::SetMutation(const ChromoNode& aMuta)
{
    __ASSERT(false);
}

TBool  MelemPx::AppendMutation(const string& aFileName)
{
    __ASSERT(false);
    return false;
}

// TODO [YB] This is short-term solutin only. We need to avoid using tree-type mutations
// in base agent APIs. To migrate to pure OSM solution, ref ds_daa_ttm 
ChromoNode  MelemPx::AppendMutation(const ChromoNode& aMuta)
{
    string resp;
    string req = Ifu::CombineIcSpec("AppendMutation#2", "1", aMuta);
    mMgr->Request(mContext, req, resp);
}

ChromoNode MelemPx::AppendMutation(TNodeType aType)
{
    __ASSERT(false);
    return ChromoNode();
}

void MelemPx::AppendMutation(const TMut& aMut)
{
    Rpcv<string>(__func__, aMut);
}

TBool  MelemPx::IsChromoAttached() const
{
    return Rpc<TBool>(__func__);
}

TBool  MelemPx::CompactChromo()
{
    __ASSERT(false);
    return false;
}

TBool MelemPx::CompactChromo(const ChromoNode& aNode)
{
    __ASSERT(false);
    return false;
}

void MelemPx::OnNodeMutated(const MUnit* aNode, const TMut& aMut, const MutCtx& aCtx)
{
    Rpcv<const MUnit*, const TMut&, const MutCtx&>(__func__, aNode, aMut, aCtx);
}

void MelemPx::OnParentMutated(MUnit* aParent, const TMut& aMut)
{
    __ASSERT(false);
};

// MParent
	
void  MelemPx::OnChildDeleting(MUnit* aChild)
{
    Rpcv<MUnit*>(__func__, aChild);
}

TBool  MelemPx::OnChildRenamed(MUnit* aComp, const string& aOldName)
{
    __ASSERT(false);
    return false;
}

TBool  MelemPx::AppendChild(MUnit* aChild)
{
    return Rpc<TBool, MUnit*>(__func__, aChild);
}

void  MelemPx::RemoveChild(MUnit* aChild)
{
    Rpcv<MUnit*>(__func__, aChild);
}


// MChild

MUnit* MelemPx::GetParent()
{
    return RpcPx<MUnit>(__func__);
}

const MUnit*  MelemPx::GetParent() const
{
    return RpcPx<const MUnit>(__func__);
}

void  MelemPx::OnParentDeleting(MUnit* aParent)
{
    Rpcv<MUnit*>(__func__, aParent);
}

void  MelemPx::SetParent(const string& aParent)
{
    __ASSERT(false);
}

void  MelemPx::SetParent(MUnit* aParent)
{
    Rpcv<MUnit*>(__func__, aParent);
}


// Debugging

void MelemPx::SaveChromo(const char* aPath) const {}

string MelemPx::GetChromoSpec() const
{
    return Rpc<string>(__func__);
}

string MelemPx::MElem_Mid() const
{
    return mMgr->Oid();
}

string MelemPx::Uid() const
{
    string res;
    Ifu::CombineUid(Mid(), GetContext(), res);
    return res;
}

void MelemPx::DumpChilds() const
{
    __ASSERT(false);
}

void MelemPx::DumpCnode(const ChromoNode& aNode) const
{
    aNode.DumpToLog(mEnv->Logger());
}

