#ifndef _reqexec_h_
#define _reqexec_h_

#include <string>
#include "reqbase.h"

using namespace std;

// Executing API by cursor
class ReqExec : public ReqBase {
    public:
	ReqExec(EnvProvider *envProv, const string& aArgs);
    protected:
	virtual string Execute();
    private:
	string DoExec();
}; 

#endif
