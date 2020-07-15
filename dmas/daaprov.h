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
	virtual Unit* CreateNode(const string& aType, const string& aName, MUnit* aMan, MEnv* aEnv) override;
	virtual Unit* GetNode(const string& aUri) override;
	virtual void AppendNodesInfo(vector<string>& aInfo) override;
	virtual void SetChromoRslArgs(const string& aRargs) override {};
	virtual void GetChromoRslArgs(string& aRargs) override {};
    private:
	vector<string> iNodesInfo;
};


#endif
