#include <dirent.h>
#include <stdexcept> 
#include <sys/stat.h>
#include <env.h> 
#include <elem.h> 

#include "rexec.h"
#include "requests.h"


ReqExec::ReqExec(EnvProvider *envProv, const string& aArgs) : ReqBase(RequestIPC::EReqID_Exec, envProv, aArgs)
{
}

string ReqExec::Execute()
{
    string res;
    try {
	res = DoExec();
    } catch (exception& e) {
	res = e.what();
    }
    return res;
}


string ReqExec::DoExec()
{
    // Separate cursor uri
    size_t curs_end = mArgs.find_first_of(RequestIPC::REQ_SEPARATOR, 0); 
    string curs = mArgs.substr(0, (curs_end == string::npos) ? mArgs.length() : curs_end);
    // Separate method name
    if (curs_end == string::npos) throw (runtime_error("Missing method name"));
    size_t mname_beg = curs_end + 1;
    size_t mname_end = mArgs.find_first_of(RequestIPC::REQ_SEPARATOR, mname_beg); 
    string mname = mArgs.substr(mname_beg, (mname_end == string::npos) ? string::npos : mname_end - mname_beg);
    // Get cursor
    Env* env = mEnvProvider->GetEnv(0);
    if (env == NULL) throw(runtime_error("Env doesn't exitst"));
    Elem* root = env->Root();
    if (root == NULL) throw(runtime_error("Cursor not found"));
    string res =  root->Name();
    return res;
}
