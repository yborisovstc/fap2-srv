#ifndef _melempx_h_
#define _melempx_h_

#include <elem.h>
#include <bclient.h>
#include "melempx.h"

/*
 * Remote environment agent. Supports of interacting to lower layer models part,
 * so is "bottom" remote env engine.
 */
class ARenv: public Elem
{
    public:
	class CompsIter: public MIterImpl {
	    friend class ARenv;
	    public:
	    CompsIter(ARenv& aElem, GUri::TElem aId, TBool aToEnd = EFalse);
	    CompsIter(ARenv& aElem);
	    CompsIter(const CompsIter& aImpl);
	    char SRel() const;
	    virtual MIterImpl* Clone();
	    virtual void Set(const MIterImpl& aImpl);
	    virtual void PostIncr();
	    virtual TBool IsEqual(const MIterImpl& aImpl) const;
	    virtual TBool IsCompatible(const MIterImpl& aImpl) const;
	    virtual MElem*  GetElem();
	    public:
	    ARenv& iElem;
	    GUri::TElem iId;
	    char iExtsrel;
	    string iExt;
	    TNMReg::iterator iChildsIter;
	    pair<TNMReg::iterator, TNMReg::iterator> iChildsRange;
	    TBool mEnd;
	};
    public:
	static const char* Type() { return "ARenv";};
	static string PEType();
	ARenv(const string& aName, MElem* aMan, MEnv* aEnv);
	ARenv(MElem* aMan, MEnv* aEnv);
	virtual ~ARenv();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual void GetCont(string& aCont, const string& aName=string()); 
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	//virtual MElem* GetNode(const GUri& aUri, GUri::const_elem_iter& aPathBase, TBool aAnywhere = EFalse);
	//virtual Iterator NodesLoc_Begin(const GUri::TElem& aElem, TBool aInclRm = EFalse);
	//virtual Iterator NodesLoc_End(const GUri::TElem& aElem, TBool aInclRm = EFalse);
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
    protected:
	void AddElemRmt(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
    protected:
	string mRenvUri;
	RenvClient mRenvClient;
	DaaPxMgr* mPxMgr;
	MelemPx* mRroot;
};

/*
 * Remote environment "upper" agent. Supports of interacting to upper layer models part
 */
class ARenvu: public Elem
{
    public:
	static const char* Type() { return "ARenvu";};
	static string PEType();
	ARenvu(const string& aName, MElem* aMan, MEnv* aEnv);
	ARenvu(MElem* aMan, MEnv* aEnv);
	virtual ~ARenvu();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
    protected:
	void Connect();
    protected:
	RenvClient mRenvClient;
	MelemPx* mRroot;
	TBool mConnected;
	DaaPxMgr* mPxMgr;
};


#endif
