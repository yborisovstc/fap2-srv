#ifndef _melempx_h_
#define _melempx_h_

#include <elem.h>
#include <bclient.h>
#include "melempx.h"
#include "mlogrecpx.h"

/*
 * Remote environment agent. Supports of interacting to lower layer models part,
 * so is "bottom" remote env engine.
 */
class ARenv: public Elem
{
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
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	// From MMutable
	virtual void DoMutation(const ChromoNode& aCromo, TBool aRunTime, TBool aCheckSafety, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
    protected:
	void AddElemRmt(const ChromoNode& aSpec, TBool aRunTime = EFalse, TBool aTrialMode = EFalse, const MElem* aCtx = NULL);
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
	ARenvu(const string& aName, MElem* aMan, MEnv* aEnv);
	ARenvu(MElem* aMan, MEnv* aEnv);
	virtual ~ARenvu();
    public:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MElem
	virtual TBool ChangeCont(const string& aVal, TBool aRtOnly = ETrue, const string& aName = string()); 
	virtual TBool IsContChangeable(const string& aName = string()) const; 
	// From MLogObserver
	virtual void AddObservable(MLogRec* aObservable);
	virtual void RemoveObservable(MLogRec* aObservable);
	virtual void OnLogAdded(long aTimeStamp, TLogRecCtg aCtg, const MElem* aNode, const std::string& aContent, TInt aMutId = 0);
	virtual void OnLogAdded(const TLog& aLog);
	virtual void OnLogRecDeleting(MLogRec* aLogRec);
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
