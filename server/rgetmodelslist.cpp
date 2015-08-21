#include <dirent.h>
#include <sys/stat.h>

#include "rgetmodelslist.h"
#include "requests.h"

const string ReqGetModelsList::MODELS_LIST_PATH = "/home/yborisov/fap_models/";

ReqGetModelsList::ReqGetModelsList(EnvProvider *envProv) : ReqBase(RequestIPC::EReqID_GetModelsList, envProv) {
}

string ReqGetModelsList::Execute() {
    return GetList(MODELS_LIST_PATH);
}

string ReqGetModelsList::GetList(const string& aPath)
{
    string res("");
    const string& dirpath = aPath;
    DIR* dp;
    struct dirent *dirp;
    struct stat filestat;
    dp = opendir(dirpath.c_str());
    if (dp != NULL) {
	while ((dirp = readdir(dp))) {
	    string fname(dirp->d_name);
	    string filepath = dirpath + "/" + fname;
	    // If the file is a directory (or is in some way invalid) we'll skip it
	    if (stat( filepath.c_str(), &filestat )) continue;
	    if (S_ISDIR( filestat.st_mode ))         continue;
	    if(fname.substr(fname.find_last_of(".") + 1) != "xml") continue;
	    res += fname;
	    res += RequestIPC::R_LIST_SEPARATOR;
	}
    }
    closedir(dp);
    return res;
}


