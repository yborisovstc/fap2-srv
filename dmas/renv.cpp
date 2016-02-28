
#include <plat.h>
#include "renv.h"
#include "../server/requests.h"
#include "melempx.h"




ARenv::CompsIter::CompsIter(ARenv& aElem, GUri::TElem aId, TBool aToEnd): iElem(aElem), iId(aId), mEnd(aToEnd)
{
    char rel = SRel();
    if (!iId.first.empty()) {
	iExtsrel = iId.first.at(iId.first.size() - 1);
	if (iExtsrel == GUri::KParentSep || iExtsrel == GUri::KNodeSep) {
	    iExt = iId.first.substr(0, iId.first.size() - 1);
	}
    }
    if (rel == GUri::KNodeSep) {
	if (iId.second.second == GUri::KTypeAny) {
	    mEnd = EFalse;
	} else {
	    mEnd = (iElem.mRroot == NULL) ? ETrue : iElem.mRroot->Name() == iId.second.second;
	}
	if (aToEnd) {
	    mEnd = aToEnd;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KParentSep || iExt.empty() || iExt == GUri::KTypeAny) {
		mEnd = EFalse;
	    } 
	    else {
		mEnd = (iElem.mRroot == NULL) ? ETrue : (iElem.mRroot->GetParent()->Name() == iExt && !iElem.mRroot->IsRemoved());
	    }
	}
    }
    else if (rel == GUri::KParentSep) {
	if (iId.second.second == GUri::KTypeAny) {
	    iChildsRange = TNMRegItRange(iElem.iChilds.begin(), iElem.iChilds.end());
	} else {
	    iChildsRange = iElem.iChilds.equal_range(iId.second.second);
	}
	if (aToEnd) {
	    iChildsIter = iChildsRange.second;
	}
	else {
	    if (iId.first.empty() || iExtsrel !=  GUri::KNodeSep || iExt.empty() || iExt == GUri::KTypeAny) {
		iChildsIter = iChildsRange.first;
		for (; iChildsIter != iChildsRange.second && iChildsIter->second->IsRemoved(); iChildsIter++); 
	    }
	    else {
		for (iChildsIter = iChildsRange.first; iChildsIter != iChildsRange.second; iChildsIter++) {
		    MElem* comp = iChildsIter->second;
		    MElem* cowner = comp->GetMan()->GetObj(cowner);
		    if (cowner->Name() == iExt && !comp->IsRemoved()) {
			break;
		    }
		}
	    }
	}
    }
};

ARenv::CompsIter::CompsIter(const CompsIter& aIt): iElem(aIt.iElem), iId(aIt.iId),
    iExtsrel(aIt.iExtsrel), iExt(aIt.iExt), iChildsIter(aIt.iChildsIter), mEnd(aIt.mEnd)
{
};

ARenv::MIterImpl* ARenv::CompsIter::Clone()
{
    return new CompsIter(*this);
}

void ARenv::CompsIter::Set(const MIterImpl& aImpl)
{
    const CompsIter& impl = dynamic_cast<const CompsIter&>(aImpl);
    iElem = impl.iElem;
    iId = impl.iId;
    iChildsIter = impl.iChildsIter;
    mEnd = impl.mEnd;
}

char ARenv::CompsIter::SRel() const
{
    return iId.second.first;
}

void ARenv::CompsIter::PostIncr()
{
    if (SRel() == GUri::KNodeSep) {
	mEnd = ETrue;
    }
    else {
	iChildsIter++;
	// Omit removed children from the look
	for (; iChildsIter != iChildsRange.second && iChildsIter->second->IsRemoved(); iChildsIter++); 
	if (!iId.first.empty() && iExtsrel == GUri::KNodeSep && !iExt.empty() && iExt != GUri::KTypeAny) {
	    for (;iChildsIter != iChildsRange.second; iChildsIter++) {
		MElem* comp = iChildsIter->second;
		MElem* cowner = comp->GetMan()->GetObj(cowner);
		if (cowner->Name() == iExt && !comp->IsRemoved()) {
		    break;
		}
	    }
	}
    }
}

TBool ARenv::CompsIter::IsCompatible(const MIterImpl& aImpl) const
{
    return ETrue;
}

TBool ARenv::CompsIter::IsEqual(const MIterImpl& aImplm) const
{
    const CompsIter& aImpl = dynamic_cast<const CompsIter&>(aImplm);
    TBool res = EFalse;
    if (IsCompatible(aImpl) && aImpl.IsCompatible(*this)) {
	res = &iElem == &(aImpl.iElem) && iId == aImpl.iId && iChildsIter == aImpl.iChildsIter && mEnd == aImpl.mEnd;
    }
    return res;
}

MElem*  ARenv::CompsIter::GetElem()
{
    MElem* res = NULL;
    if (SRel() == GUri::KNodeSep) {
	if (!mEnd) {
	    res = iElem.mRroot;
	}
    }
    else if (SRel() == GUri::KParentSep) {
	res = iChildsIter->second;
    }
    return res;
}



string ARenv::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ARenv::ARenv(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mRroot(NULL)
{
    SetParent(Type());
}

ARenv::ARenv(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mRroot(NULL)
{
    SetParent(Elem::PEType());
}

ARenv::~ARenv() {
    for (vector<DaaProxy*>::iterator it = mProxies.begin(); it != mProxies.end(); it++) {
	DaaProxy* px = *it;
	delete px;
    }
}

void *ARenv::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool ARenv::IsContChangeable(const string& aName) const
{
    return ETrue;
}

TBool ARenv::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    if (aVal != mRenvUri) {
	mRenvUri = aVal;
	try {
	    mRenvClient.Connect("");
	} catch (exception& e) {
	    Logger()->Write(MLogRec::EErr, this, "Connecting to [%s] failed", aVal.c_str());
	    res = EFalse;
	}
	if (res) {
	    if (aRtOnly) {
		iMan->OnContentChanged(*this);
	    } else {
		iMan->OnCompChanged(*this);
	    }
	}
    }
    return res;
}

void ARenv::GetCont(string& aCont, const string& aName)
{
    aCont = mRenvUri;
}

void ARenv::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    const ChromoNode& mroot = aMutSpec;
    if (mroot.Begin() == mroot.End()) return;
    TInt tord = 0;
    TInt lim = 0;
    TBool isattached = EFalse;
    if (EN_MUT_LIM) {
	tord = iEnv->ChMgr()->GetSpecMaxOrder();
	lim = iEnv->ChMgr()->GetLim();
	isattached = IsChromoAttached();
    }
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++)
    {
	ChromoNode rno = (*rit);
	// Omit inactive mutations
	if (iEnv->ChMgr()->EnableOptimization() && rno.AttrExists(ENa_Inactive)) {
	    if (!aRunTime) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	Logger()->SetContextMutId(rno.LineId());
	TInt order = rno.GetOrder();
	// Avoiding mutations above limit. Taking into account only attached chromos.
	if (EN_MUT_LIM && isattached && tord > 0 && order > tord - lim) {
	    if (!aRunTime && !aTrialMode) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	if (rno.AttrExists(ENa_Targ)) {
	    // Targeted mutation, propagate downward, i.e redirect to comp owning the target
	    // ref ds_mut_osm_linchr_lce
	    MElem* ftarg = GetNode(rno.Attr(ENa_Targ));
	    // Mutation is not local, propagate downward
	    if (ftarg != NULL) {
		ChromoNode& troot = ftarg->Mutation().Root();
		ChromoNode madd = troot.AddChild(rno, ETrue, ETrue);
		madd.RmAttr(ENa_Targ);
		// Redirect the mut to target: no run-time to keep the mut in internal nodes
		// Propagate till target owning comp if run-time to keep hidden all muts from parent 
		ftarg->Mutate(EFalse, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Cannot find target node [%s]", rno.Attr(ENa_Targ).c_str());
	    }
	} else {
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		AddElemRmt(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Change) {
		ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else if (rnotype == ENt_Cont) {
		DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else if (rnotype == ENt_Move) {
		MoveNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Import) {
		ImportNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Rm) {
		RmNode(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Mutating - unknown mutation type [%d]", rnotype);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

#if 0
MElem* ARenv::GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere) 
{
    MElem* res = NULL;
    GUri::const_elem_iter uripos = aPathBase;
    GUri::TElem elem = *uripos;
    TBool anywhere = aAnywhere;
    if (elem.second.second == "..") {
	if (iMan != NULL) {
	    if (++uripos != aUri.Elems().end()) {
		res = iMan->GetNode(aUri, uripos);
	    }
	    else {
		res = iMan;
	    }
	}
	else {
	    __ASSERT(EFalse);
	    Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - path to top of root", aUri.GetUri().c_str());
	}
    }
    else {
	if (!anywhere && elem.second.second == GUri::KTypeAnywhere) {
	    elem = GUri::Elem(GUri::KParentSep, GUri::KTypeAny, GUri::KTypeAny);
	    anywhere = ETrue;
	}
	TBool isnative = elem.second.first == GUri::KSepNone;
	if (isnative) {
	    // Native node, not embedded to hier, to get from environment, ref uc_045
	    uripos++;
	    if (uripos != aUri.Elems().end()) {
		Elem* node = Provider()->GetNode(elem.second.second);
		if (node != NULL) {
		    res = node->GetNode(aUri, uripos);
		}
	    }
	    else {
		res = Provider()->GetNode(elem.second.second);
	    }
	}
	else {
	    // Redirect the request to remote root proxy
	    res = mRroot->GetNode(aUri, uripos);
	}
	if (res == NULL && anywhere && uripos != aUri.Elems().end()) {
	    // Try to search in all nodes
	    elem = GUri::Elem(GUri::KNodeSep, GUri::KTypeAny, GUri::KTypeAny);
	    Iterator it = NodesLoc_Begin(elem);
	    Iterator itend = NodesLoc_End(elem);
	    for (; it != itend; it++) {
		MElem* node = *it;
		MElem* res1 = node->GetNode(aUri, uripos, anywhere);
		if (res1 != NULL) {
		    if (res == NULL) {
			res = res1;
		    }
		    else {
			res = NULL;
			Logger()->Write(MLogRec::EErr, this, "Getting node [%s] - multiple choice", aUri.GetUri().c_str());
			break;
		    }
		}
	    }
	}
    }
    return res;
}
#endif

Elem::Iterator ARenv::NodesLoc_Begin(const GUri::TElem& aId, TBool aInclRm)
{
    return Iterator(new CompsIter(*this, aId));
}

Elem::Iterator ARenv::NodesLoc_End(const GUri::TElem& aId, TBool aInclRm)
{
    return Iterator(new CompsIter(*this, aId, ETrue));
}


void ARenv::AddElemRmt(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode)
{
    string env;
    string spec;
    aSpec.ToString(spec);
    // Create remote env
    TBool res = mRenvClient.Request("EnvProvider", "CreateEnv,1," + spec, env);
    if (res) {
	// Get current session id
	string sid;
	res = iEnv->GetEVar("SID", sid);
	if (res) {
	    string resp;
	    // Set session id and this Uid to remote env as PrimarySid and PrimaryUid
	    res = mRenvClient.Request(env, "SetEVar,1,PrimarySID," + sid, resp);
	    res = res && mRenvClient.Request(env, "SetEVar,1,PrimaryUid," + GetUri(), resp);
	    if (res) {
		// Create remote model
		res = mRenvClient.Request(env, "ConstructSystem", resp);
		if (res) {
		    Logger()->Write(MLogRec::EInfo, this, "Added node to remote env [%s]", mRenvUri.c_str());
		    // Get remote root
		    string rroot;
		    res = mRenvClient.Request(env, "Root", rroot);
		    if (res) {
			Logger()->Write(MLogRec::EInfo, this, "Getting remote env root, resp: %s", rroot.c_str());
			// Create proxy for remote root, bind proxy to component
			MelemPx* px = new MelemPx(iEnv, this, rroot);
			mRroot = px;
		    } else {
			Logger()->Write(MLogRec::EErr, this, "Failed getting remote env root, resp: %s", rroot.c_str());
		    }
		} else {
		    Logger()->Write(MLogRec::EErr, this, "Error adding node to remote env [%s], resp [%s]", mRenvUri.c_str(), resp.c_str());
		}
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Error creating remote env [%s]: %s", mRenvUri.c_str(), env.c_str());
	    }
	} else {
	    Logger()->Write(MLogRec::EErr, this, "Error creating remote model: session ID isn't set");
	}
    } else {
	Logger()->Write(MLogRec::EErr, this, "Error creating remote model env: %s", env.c_str());
    }
}

TBool ARenv::Request(const string& aContext, const string& aReq, string& aResp)
{
    return mRenvClient.Request(aContext, aReq, aResp);
}







string ARenvu::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ARenvu::ARenvu(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mRroot(NULL), mConnected(EFalse)
{
    SetParent(Type());
}

ARenvu::ARenvu(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mRroot(NULL), mConnected(EFalse)
{
    SetParent(Elem::PEType());
}

ARenvu::~ARenvu() {
    for (vector<DaaProxy*>::iterator it = mProxies.begin(); it != mProxies.end(); it++) {
	DaaProxy* px = *it;
	delete px;
    }
}

void *ARenvu::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

TBool ARenvu::IsContChangeable(const string& aName) const
{
    return ETrue;
}

TBool ARenvu::ChangeCont(const string& aVal, TBool aRtOnly, const string& aName)
{
    TBool res = ETrue;
    Connect();
    return res;
}

void ARenvu::DoMutation(const ChromoNode& aMutSpec, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode, const MElem* aCtx)
{
    const ChromoNode& mroot = aMutSpec;
    if (mroot.Begin() == mroot.End()) return;
    TInt tord = 0;
    TInt lim = 0;
    TBool isattached = EFalse;
    if (EN_MUT_LIM) {
	tord = iEnv->ChMgr()->GetSpecMaxOrder();
	lim = iEnv->ChMgr()->GetLim();
	isattached = IsChromoAttached();
    }
    for (ChromoNode::Const_Iterator rit = mroot.Begin(); rit != mroot.End(); rit++)
    {
	ChromoNode rno = (*rit);
	// Omit inactive mutations
	if (iEnv->ChMgr()->EnableOptimization() && rno.AttrExists(ENa_Inactive)) {
	    if (!aRunTime) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	Logger()->SetContextMutId(rno.LineId());
	TInt order = rno.GetOrder();
	// Avoiding mutations above limit. Taking into account only attached chromos.
	if (EN_MUT_LIM && isattached && tord > 0 && order > tord - lim) {
	    if (!aRunTime && !aTrialMode) {
		iChromo->Root().AddChild(rno);
	    }
	    continue;
	}
	if (rno.AttrExists(ENa_Targ)) {
	    // Targeted mutation, propagate downward, i.e redirect to comp owning the target
	    // ref ds_mut_osm_linchr_lce
	    MElem* ftarg = GetNode(rno.Attr(ENa_Targ));
	    // Mutation is not local, propagate downward
	    if (ftarg != NULL) {
		ChromoNode& troot = ftarg->Mutation().Root();
		ChromoNode madd = troot.AddChild(rno, ETrue, ETrue);
		madd.RmAttr(ENa_Targ);
		// Redirect the mut to target: no run-time to keep the mut in internal nodes
		// Propagate till target owning comp if run-time to keep hidden all muts from parent 
		ftarg->Mutate(EFalse, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Cannot find target node [%s]", rno.Attr(ENa_Targ).c_str());
	    }
	} else {
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		AddElem(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Change) {
		ChangeAttr(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else if (rnotype == ENt_Cont) {
		DoMutChangeCont(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else if (rnotype == ENt_Move) {
		MoveNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Import) {
		ImportNode(rno, aRunTime, aTrialMode);
	    }
	    else if (rnotype == ENt_Rm) {
		RmNode(rno, aRunTime, aCheckSafety, aTrialMode);
	    }
	    else {
		Logger()->Write(MLogRec::EErr, this, "Mutating - unknown mutation type [%d]", rnotype);
	    }
	    Logger()->SetContextMutId();
	}
    }
}

TBool ARenvu::Request(const string& aContext, const string& aReq, string& aResp)
{
    return mRenvClient.Request(aContext, aReq, aResp);
}

void ARenvu::Connect()
{
    string psid, puid;
    TBool res = iEnv->GetEVar("PrimarySID", psid);
    res = res && iEnv->GetEVar("PrimaryUid", puid);
    if (res) {
	try {
	    mRenvClient.Connect("");
	    res = ETrue;
	} catch (exception& e) {
	    Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed");
	}
	if (res) {
	    string resp;
	    res = mRenvClient.Request("EnvProvider", "AttachEnv,1," + psid, resp);
	    if (res) {
		// Get primary env agent
		string root, pagt;
		res = mRenvClient.Request("MEnv#0", "Root", root);
		res = res && mRenvClient.Request(root, "GetNode,1," + puid, pagt);
		if (res) {
		    mConnected = ETrue;
		    MelemPx* px = new MelemPx(iEnv, this, pagt);
		    if (px != NULL) {
			iMan = px;
		    }
		    // Create proxy for primary uid
		} else {
		    Logger()->Write(MLogRec::EErr, this, "Primary agent access failed: %s", resp.c_str());
		}
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed: cannot attach to env#%s", psid.c_str());
	    }
	}
    } else {
	Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed: missing primary session id");
    }
}

string ARenvu::Uid() const
{
    return Elem::Uid();
}
