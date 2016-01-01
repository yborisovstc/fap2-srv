#ifndef _reqbase_h_
#define _reqbase_h_

#include <string>
#include <env.h>

using namespace std;
class ReqBase {
    public:
	class EnvProvider {
	    public:
		virtual void CreateEnv(const string& aChromo) = 0;
		virtual Env* GetEnv(int aEnvId) = 0;
	};
    protected:
	int mId;
	string mArgs;
	string mRes;
	EnvProvider *mEnvProvider;
    protected:
	ReqBase(int id, EnvProvider *envProv);
	ReqBase(int id, EnvProvider *envProv, string args);
	virtual void ParseArgs() {};
	virtual void SetRes(string res);
	virtual string Execute() = 0;
    public:
	virtual string Exec();
	virtual string GetRes();
}; 

#endif
