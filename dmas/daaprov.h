#ifndef __FAP2_SRV_DAAPROV_H
#define __FAP2_SRV_DAAPROV_H

#include <prov.h>

// Agents Provider for Dynamic Async Agents (DAA) 
//
class DaaProv: public GProvider
{
    public:
	DaaProv(const string &aName, MEnv* aEnv);
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, MElem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual void AppendNodesInfo(vector<string>& aInfo);
    private:
	vector<string> iNodesInfo;
};


#endif
