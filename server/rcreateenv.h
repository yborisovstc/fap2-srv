#ifndef _reqcreateenv_h_
#define _reqcreateenv_h_

#include <string>
#include "reqbase.h"

using namespace std;

class ReqCreateEnv : public ReqBase {
    public:
	ReqCreateEnv(EnvProvider *envProv, const string& aChromo);
    protected:
	virtual string Execute();
    private:
	const string& mChromo;
}; 

#endif
