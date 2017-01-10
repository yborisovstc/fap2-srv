
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


const string ARenv::KRemoteUid = "Remote_Uid";

string ARenv::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ARenv::ARenv(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mRroot(NULL)
{
    SetParent(Type());
    mPxMgr = new DaaPxMgr(aEnv, this, mRenvClient);
    ChangeCont(string(), ETrue, KRemoteUid);
}

ARenv::ARenv(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mRroot(NULL)
{
    SetParent(Elem::PEType());
    mPxMgr = new DaaPxMgr(aEnv, this, mRenvClient);
    ChangeCont(string(), ETrue, KRemoteUid);
}

ARenv::~ARenv() {
    // Remove the comps first before proxies to support normal deletion of model, ref ds_daa_powrd
    // Plainly delete all comps and clear comps registry. There is only one comp - proxy. 
    // It doesn't have relation // comp-owner established, so the base agent mechanism of
    // comps removal will not work (it assumes that comp notifies owner OnCompDeleting).
    for (vector<MElem*>::reverse_iterator it = iComps.rbegin(); it != iComps.rend(); it++) {
	MElem* comp = *it;
	comp->Delete();
    };
    iComps.clear();
    delete mPxMgr;
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
    if (aName == KRemoteUid) {
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
		    iMan->OnChanged(*this);
		} else {
		    iMan->OnCompChanged(*this);
		}
	    }
	}
    }
    Elem::ChangeCont(aVal, aRtOnly, aName);
    return res;
}

/*
TBool ARenv::GetCont(string& aCont, const string& aName) const
{
    aCont = mRenvUri;
    return ETrue;
}
*/

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
		rno.RmAttr(ENa_Targ);
		ftarg->AppendMutation(rno);
		// Redirect the mut to target: no run-time to keep the mut in internal nodes
		// Propagate till target owning comp if run-time to keep hidden all muts from parent 
		ftarg->Mutate(EFalse, aCheckSafety, aTrialMode, aRunTime ? GetCompOwning(ftarg) : aCtx);
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Cannot find target node [%s]", rno.Attr(ENa_Targ).c_str());
	    }
	} else {
	    TNodeType rnotype = rno.Type();
	    if (rnotype == ENt_Node) {
		AddElemRmt(rno, aRunTime, aTrialMode, aCtx);
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

/*
Elem::Iterator ARenv::NodesLoc_Begin(const GUri::TElem& aId, TBool aInclRm)
{
    return Iterator(new CompsIter(*this, aId));
}

Elem::Iterator ARenv::NodesLoc_End(const GUri::TElem& aId, TBool aInclRm)
{
    return Iterator(new CompsIter(*this, aId, ETrue));
}
*/


void ARenv::AddElemRmt(const ChromoNode& aSpec, TBool aRunTime, TBool aTrialMode, const MElem* aCtx)
{
    string env;
    string spec;
    aSpec.ToString(spec);
    // Check if client is connected
    if (mRenvClient.IsConnected()) {
	// Create remote env
	TBool res = mRenvClient.Request("EnvProvider", "CreateEnv,1," + spec, env);
	if (res) {
	    // Get current server uri
	    string sid, eid;
	    res = iEnv->GetEVar("SID", sid);
	    res = res && iEnv->GetEVar("EID", eid);
	    if (res) {
		string resp;
		// Set server id, env id and this agent Uid to remote env as PrimarySid and PrimaryUid
		res = mRenvClient.Request(env, "SetEVar,1,PrimarySID," + sid, resp);
		res = res && mRenvClient.Request(env, "SetEVar,1,PrimaryEID," + eid, resp);
		res = res && mRenvClient.Request(env, "SetEVar,1,PrimaryUid," + GetUri(), resp);
		// Set session Id
		string rsid;
		res = res && mRenvClient.Request(env, "GetEVar,1,SSID", rsid);
		if (res) {
		    mRenvClient.SetRmtSID(rsid);
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
			    //MelemPx* px = new MelemPx(iEnv, mPxMgr, rroot);
			    MProxy* mpx = mPxMgr->CreateProxy(MElem::Type(), rroot);
			    MElem* px = (MElem*) mpx->GetIface(MElem::Type());
			    //MelemPx* px = dynamic_cast<MelemPx*>(mpx);
			    res = AppendComp(px);
			    //mRroot = px;
			    if (!aRunTime) {
				// Copy just top node, not recursivelly, ref ds_daa_chrc_va
				iChromo->Root().AddChild(aSpec, ETrue, EFalse);
				NotifyNodeMutated(aSpec, aCtx);
			    }
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
    } else { // Checking client is connected
	Logger()->Write(MLogRec::EErr, this, "There is no connection to remote env yet. Check content [Remote_Uri] and remote running");
    }
}





string ARenvu::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ARenvu::ARenvu(const string& aName, MElem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv), mRroot(NULL), mConnected(EFalse),
    mPxMgr(NULL), mRlog(NULL), mLogObsbl(NULL)
{
    SetParent(Type());
    mPxMgr = new DaaPxMgr(aEnv, this, mRenvClient);
    //Connect();
}

ARenvu::ARenvu(MElem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv), mRroot(NULL), mConnected(EFalse), mPxMgr(NULL),
    mRlog(NULL), mLogObsbl(NULL)
{
    SetParent(Elem::PEType());
    // Connect(); Don't connect if creating just native agent
    mPxMgr = new DaaPxMgr(aEnv, this, mRenvClient);
}

ARenvu::~ARenvu()
{
    // Remove the comps first before proxies to support normal deletion of model, ref ds_daa_powrd
    vector<MElem*>::reverse_iterator it = iComps.rbegin();
    while (it != iComps.rend()) {
	delete *it;
	it = iComps.rbegin();
    }
    iComps.clear();
    if (iMan != NULL) {
	iMan->OnCompDeleting(*this, EFalse);
	iMan = NULL;
    }
    if (mLogObsbl != NULL) {
	mLogObsbl->RemoveLogObserver(this);
    }
    if (mRlog != NULL) {
	delete mRlog;
    }
    delete mPxMgr;
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

void ARenvu::Connect()
{
    string psid, puid, peid;
    TBool res = iEnv->GetEVar("PrimarySID", psid);
    res = res && iEnv->GetEVar("PrimaryEID", peid);
    res = res && iEnv->GetEVar("PrimaryUid", puid);
    if (res) {
	try {
	    mRenvClient.Connect(psid);
	    res = ETrue;
	} catch (exception& e) {
	    Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed");
	}
	if (res) {
	    string rsid;
	    res = mRenvClient.Request("EnvProvider", "GetId,1", rsid);
	    string resp;
	    //res = mRenvClient.Request("EnvProvider", "AttachEnv,1," + psid, resp);
	    if (res) {
		// Get primary env
		string rlog;
		// Get logger of primary env
		res = mRenvClient.Request(peid, "Logger,1", rlog);
		if (res) {
		    MlogrecPx* px = new MlogrecPx(iEnv, mPxMgr, rlog);
		    if (px != NULL) {
			mRlog = px;
			mRlog->Write(TLog(EInfo, this) + "Remote root created: " + Name());
			if (Logger()->AddLogObserver(this)) {
			    mLogObsbl = Logger();
			}
		    }
		    // Create proxy for primary uid
		} else {
		    Logger()->Write(TLog(EErr, this) + "Primary logger access failed: " + resp);
		}
		mRenvClient.SetRmtSID(rsid);
		// Get primary env agent
		string root, pagt;
		res = mRenvClient.Request(peid, "Root", root);
		res = res && mRenvClient.Request(root, "GetNode,1," + puid, pagt);
		if (res) {
		    mConnected = ETrue;
		    MelemPx* px = new MelemPx(iEnv, mPxMgr, pagt);
		    if (px != NULL) {
			iMan = px;
		    }
		    // Create proxy for primary uid
		} else {
		    Logger()->Write(MLogRec::EErr, this, "Primary agent access failed: %s", resp.c_str());
		}
	    } else {
		Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed: missing primary session id");
	    }
	}
    } else {
	Logger()->Write(MLogRec::EErr, this, "Connecting to primary environment failed: missing primary env ids");
    }
}

void ARenvu::AddObservable(MLogRec* aObservable)
{
}

void ARenvu::RemoveObservable(MLogRec* aObservable)
{
}

void ARenvu::OnLogAdded(long aTimeStamp, MLogRec::TLogRecCtg aCtg, const MElem* aNode, const std::string& aContent, TInt aMutId)
{
}

void ARenvu::OnLogAdded(const TLog& aLog)
{
    if (mRlog != NULL) {
	mRlog->Write(aLog);
    }
}

void ARenvu::OnLogRecDeleting(MLogRec* aLogRec)
{
}

