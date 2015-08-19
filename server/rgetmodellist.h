#ifndef _reqgetmodellist_h_
#define _reqgetmodellist_h_

#include <string>
#include "reqbase.h"

using namespace std;

class ReqGetModelList : public ReqBase {
    private:
	static const string MODELS_LIST_PATH;
    public:
	ReqGetModelList(EnvProvider *envProv);
    protected:
	virtual string Execute();
    private:
	string GetList(const string& aPath);
}; 

#endif
