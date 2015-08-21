#ifndef _reqgetmodelslist_h_
#define _reqgetmodelslist_h_

#include <string>
#include "reqbase.h"

using namespace std;

class ReqGetModelsList : public ReqBase {
    private:
	static const string MODELS_LIST_PATH;
    public:
	ReqGetModelsList(EnvProvider *envProv);
    protected:
	virtual string Execute();
    private:
	string GetList(const string& aPath);
}; 

#endif
