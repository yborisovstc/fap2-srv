
#include "daaprov.h"

#include "renv.h"



DaaProv::DaaProv(const string &aName, MEnv* aEnv): GProvider(aName, aEnv)
{
}

Elem* DaaProv::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    if (aType.compare(ARenv::Type()) == 0) {
	res = new ARenv(aName, aMan, aEnv);
    }
    else if (aType.compare(ARenvu::Type()) == 0) {
	res = new ARenvu(aName, aMan, aEnv);
    }
    return res;
}

Elem* DaaProv::GetNode(const string& aUri){
    MProvider* prov = iEnv->Provider();
    Elem* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	Elem* parent = NULL;
	if (aUri.compare(ARenv::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ARenv(NULL, iEnv);
	}
	else if (aUri.compare(ARenvu::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ARenvu(NULL, iEnv);
	}
	if (res != NULL) {
	    if (parent != NULL) {
		parent->AppendChild(res);
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}


void DaaProv::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(ARenv::Type());
}

