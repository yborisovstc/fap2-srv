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
	virtual MIface* GetIface(const string& aName) override;
	virtual const MIface* GetIface(const string& aName) const override;
	virtual string GetUid() const override;
	// From MLogRec
	static const char* Type() { return "MLogRec";};
	virtual void WriteFormat(const char* aFmt,...) override;
	virtual void Write(TLogRecCtg aCtg, const MUnit* aNode, const char* aFmt,...) override;
	virtual void Write(TLogRecCtg aCtg, MUnit* aNode, const ChromoNode& aMut, const char* aFmt,...) override;
	virtual void Flush() override;
	virtual void SetContextMutId(TInt aMutId = -1) override;
	virtual TBool AddLogObserver(MLogObserver* aObs) override;
	virtual void RemoveLogObserver(MLogObserver* aObs) override;
	virtual TInt GetStat(TLogRecCtg aCtg) const override;
	virtual void Write(const TLog& aRec) override;
    public:
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes) override;
	virtual string Uid() const override;
	virtual string Mid() const override;
};

#endif
