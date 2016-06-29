#ifndef _MLOGRECPX_h_
#define _MLOGRECPX_h_

#include "daaproxy.h"
#include <mlog.h>
#include <ifu.h>

class MlogrecPx : public DaaProxy, public MLogRec
{
    public:
	MlogrecPx(MEnv* aEnv, MProxyMgr* aMgr, const string& aContext);
	virtual ~MlogrecPx();
    public:
	// From MProxy
	virtual MIface* GetIface(const string& aName);
	virtual const MIface* GetIface(const string& aName) const;
	virtual string GetUid() const;
	// From MLogRec
	static const char* Type() { return "MLogRec";};
	virtual void WriteFormat(const char* aFmt,...);
	virtual void Write(TLogRecCtg aCtg, const MElem* aNode, const char* aFmt,...);
	virtual void Write(TLogRecCtg aCtg, MElem* aNode, const ChromoNode& aMut, const char* aFmt,...);
	virtual void Flush();
	virtual void SetContextMutId(TInt aMutId = -1);
	virtual TBool AddLogObserver(MLogObserver* aObs);
	virtual void RemoveLogObserver(MLogObserver* aObs);
	virtual TInt GetStat(TLogRecCtg aCtg) const;
	virtual void Write(const TLog& aRec);
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Uid() const;
	virtual string Mid() const;
};

#endif
