
#include "daaprov.h"

#include "renv.h"
#include "daapxprov.h"



DaaProv::DaaProv(const string &aName, MEnv* aEnv): GProvider(aName, aEnv)
{
}

Unit* DaaProv::CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv)
{
    Unit* res = NULL;
    if (aType.compare(ARenv::Type()) == 0) {
	res = new ARenv(aName, aMan, aEnv);
    }
    else if (aType.compare(ARenvu::Type()) == 0) {
	res = new ARenvu(aName, aMan, aEnv);
    }
    else if (aType.compare(ADaaPxProv::Type()) == 0) {
	res = new ADaaPxProv(aName, aMan, aEnv);
    }
    if (res != NULL) {
	Unit* parent = GetNode(aType);
	if (parent != NULL) {
	    MElem* eparent = parent->GetObj(eparent);
	    if (eparent != NULL) {
		eparent->AppendChild(res);
	    }
	}
    }
    return res;
}

Unit* DaaProv::GetNode(const string& aUri){
    MProvider* prov = iEnv->Provider();
    Unit* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	MUnit* parent = NULL;
	if (aUri.compare(ARenv::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ARenv(NULL, iEnv);
	}
	else if (aUri.compare(ARenvu::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ARenvu(NULL, iEnv);
	}
	else if (aUri.compare(ADaaPxProv::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ADaaPxProv(NULL, iEnv);
	}
	if (res != NULL) {
	    if (parent != NULL) {
		MElem* eparent = parent->GetObj(eparent);
		if (eparent != NULL) {
		    eparent->AppendChild(res);
		}
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}

void DaaProv::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(ARenv::Type());
    aInfo.push_back(ARenvu::Type());
    aInfo.push_back(ADaaPxProv::Type());
}

