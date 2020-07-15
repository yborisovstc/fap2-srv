#ifndef _melempx_h_
#define _melempx_h_

#include <elem.h>
#include <bclient.h>
#include "melempx.h"
#include "mlogrecpx.h"

/** @brief Remote environment agent. Supports of interacting to lower layer models part,
 * so is "bottom" remote env engine.
 */
class ARenv: public Elem
{
    public:
	static const char* Type() { return "ARenv";};
	static string PEType();
	ARenv(const string& aName, MUnit* aMan, MEnv* aEnv);
	ARenv(MUnit* aMan, MEnv* aEnv);
	virtual ~ARenv();
    public:
	// From MElem
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	// From MMutable
	void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx) override;
    protected:
	void AddElemRmt(const ChromoNode& aSpec, TBool aRunTime /*EFalse*/, TBool aTrialMode /*EFalse*/, const MutCtx& aCtx);
    protected:
	string mRenvUri;
	RenvClient mRenvClient;
	DaaPxMgr* mPxMgr;
	MelemPx* mRroot;
	static const string KRemoteUid;
};

/*
 * Remote environment "upper" agent. Supports of interacting to upper layer models part
 */
class ARenvu: public Elem, public MLogObserver
{
    public:
	static const char* Type() { return "ARenvu";};
	static string PEType();
	ARenvu(const string& aName, MUnit* aMan, MEnv* aEnv);
	ARenvu(MUnit* aMan, MEnv* aEnv);
	virtual ~ARenvu();
    public:
	// From MElem
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	// From MLogObserver
	virtual void AddObservable(MLogRec* aObservable) override;
	virtual void RemoveObservable(MLogRec* aObservable) override;
	virtual void OnLogAdded(long aTimeStamp, TLogRecCtg aCtg, const MUnit* aNode, const std::string& aContent, TInt aMutId = 0) override;
	virtual void OnLogAdded(const TLog& aLog) override;
	virtual void OnLogRecDeleting(MLogRec* aLogRec) override;
    protected:
	void Connect();
    protected:
	RenvClient mRenvClient;
	MelemPx* mRroot;
	TBool mConnected;
	DaaPxMgr* mPxMgr;
	MlogrecPx* mRlog;
	MLogRec* mLogObsbl; // Log observable
};


#endif
