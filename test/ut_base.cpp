#include <stdlib.h>
#include "../client/bclient.h"
#include "../server/requests.h"
#include "../server/sserver.h"
#include "../server/sclient.h"
#include <sys/types.h>
#include <signal.h>
#include <env.h>
#include <elem.h>
#include <mvert.h>
#include <daaprov.h>
#include <iostream>
#include <fstream>

#include <cppunit/extensions/HelperMacros.h>

/*
 * To run particular test suite: ./ut-fap2srv-lib [test_suite_name]
 * for instance ./ut-fap2srv-lib Ut_ExecMagt
 */


// Wait until server run
bool WaitSrv()
{
    bool srv_run = false;
    for (int cta = 0; cta < 10; cta++) {
	sleep(1);
	int rr = system("ps -e | grep fap2-srv > /dev/null");
	if (rr == 0) {
	    srv_run = true; break;
	}
    }
    return srv_run;
}

class Ut_base : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_base);
    CPPUNIT_TEST(test_Connect);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Connect();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_base, "Ut_base");


void Ut_base::setUp()
{
}

void Ut_base::tearDown()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


/* Test suite to verify creating model in remote environment
 * Run fap2 server before launching test
 */
class Ut_CreateEnv : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_CreateEnv);
    CPPUNIT_TEST(test_CreateModel);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_CreateModel();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_CreateEnv, "Ut_CreateEnv");


void Ut_CreateEnv::setUp()
{
}

void Ut_CreateEnv::tearDown()
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}


void Ut_base::test_Connect()
{
    printf("\n === Test of connecting to server\n");
    if (fork() != 0) {
	printf("\n === Child process\n");
	// Create client
	BaseClient* client = new BaseClient();
	// Wait until server run
	bool srv_run = WaitSrv();
	CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
	try {
	    client->Connect("");
	} catch (exception& e) {
	    CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
	}
	printf("Client connected to the server\n");
    } else {
	printf("\n === Starting server\n");
	int exit_status = system("../server/fap2-srv");
	printf("\n === Server exited, status: %i\n", exit_status);
	exit(0);
    }
}


const string KChromo_1 = "<node id=\"Root\" parent=\"Elem\"></node>";
const string KMeth_CreateEnv = "CreateEnv";

// Test of request "Create model"
void Ut_CreateEnv::test_CreateModel()
{
    printf("\n === Test of creating model in remote env\n");
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    string resp;
    string cenv;
    // Issue request for creating env
    bool res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + KChromo_1, cenv);
    printf("Create model -- Response: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating remote env failed: " + cenv, res);
    // Issue request for creating model
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Constructing system -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Constructing system failed: " + resp, res);
    // Issue request for root
    res = client->Request(cenv, "Root", resp);
    printf("Getting root -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -get root- failed", res);
    // Issue request for root name
    res = client->Request(resp, "Name", resp);
    printf("Getting root name -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -get root name- failed", res);
    client->Disconnect();
    delete client;
}


/* Test suite to verify MElem iface requests
 * Run fap2 server before launching test
 */
class Ut_ExecMagt : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_ExecMagt);
    CPPUNIT_TEST(test_Melem);
    CPPUNIT_TEST_SUITE_END();
public:
    virtual void setUp();
    virtual void tearDown();
private:
    void test_Melem();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_ExecMagt, "Ut_ExecMagt");


void Ut_ExecMagt::setUp() { }

void Ut_ExecMagt::tearDown() { }

// Test of request "Exec Melem iface methods"
void Ut_ExecMagt::test_Melem()
{
    printf("\n === Test of Exec Melem iface methods\n");
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    string resp;
    string sid;
    bool res = client->Request("EnvProvider", "GetId", sid);
    CPPUNIT_ASSERT_MESSAGE("Request -GetId- failed", res);
    printf("Getting session1 id -- Response: %s\n", sid.c_str());
    // Creating env
    string cenv;
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + KChromo_1, cenv);
    printf("Create model -- Response: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -create_model- failed", res);
    // Creating model
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Constructing model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Constructing model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root -- Response: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -get root- failed", res);
    // Checking root name
    res = client->Request(root, "Name", resp);
    printf("Getting root name -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -get root name- failed", res);
    // Mutating
    res = client->Request(root, "Mutate,1,<node><node id=\"node_1\" parent=\"Elem\"></node></node>,false,true,true", resp);
    printf("Mutating root -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -mutating root- failed", res);
    // Checking new node
    string node_1;
    res = client->Request(root, "GetNode,1,./node_1", node_1);
    printf("Getting node_1 -- Response: %s\n", node_1.c_str());
    CPPUNIT_ASSERT_MESSAGE("Checking new node, gettin node failed", res);
    res = client->Request(node_1, "Name", resp);
    printf("Node_1 name -- Response: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Checking new node, gettin node name failed", res);

    // Checking attaching to model
    BaseClient* client2 = new BaseClient();
    try {
	client2->Connect("");
	printf("Client2 connected to the server\n");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    res = client2->Request("EnvProvider", "AttachEnv,1," + sid, resp);
    CPPUNIT_ASSERT_MESSAGE("Attaching session2 to session1 env failed: " + resp, res);
    // Getting root via attachment
    string root2;
    res = client2->Request(cenv, "Root", root2);
    CPPUNIT_ASSERT_MESSAGE("Gettng root via attach failed: " + root2, res);
    printf("Getting root via attach -- Response: %s\n", root2.c_str());

    client->Disconnect();
    client2->Disconnect();
    delete client;
    delete client2;
}




/* Test suite to verify remote env agent
*/
class Ut_Renva : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Renva);
    CPPUNIT_TEST(test_Renva_Cre);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Renva_Cre();
    private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Renva, "Ut_Renva");


void Ut_Renva::setUp()
{
}

void Ut_Renva::tearDown() { }

/* Test of request "Exec Melem iface methods"
 * Actually this schema is not working, there is no sense
 * to create remote model because there is no means of remote
 * model communicating to local primary model
 * So this test is for very limited purpose only: to verify
 * of functioning of primary remote agent and MElem proxy
 * TODO [YB] Currently this test in not working because design of Renv agent
 * required that the primary environment is also in server context
 */
void Ut_Renva::test_Renva_Cre()
{
    printf("\n === Test of Creating remote env agent\n");
    // Create model
    iEnv = new Env("ut_renva_cre.xml", "ut_renva_cre.txt");
    CPPUNIT_ASSERT_MESSAGE("Fail to create Env", iEnv != 0);
    iEnv->ImpsMgr()->ResetImportsPaths();
    // TODO [YB] To correct modules path
    //iEnv->ImpsMgr()->AddImportsPaths("../modules");
    // Dinamic async agents specific nodes provider
    DaaProv* daaprov = new DaaProv("DaaProv", iEnv);
    iEnv->AddProvider(daaprov);
    iEnv->ConstructSystem();
    Elem* root = iEnv->Root();
    CPPUNIT_ASSERT_MESSAGE("Fail to get local root", root != 0);
    // Getting local remote env agent
    MElem* renv = root->GetNode("./Renv"); 
    CPPUNIT_ASSERT_MESSAGE("Fail to get Renv", renv != 0);
    // Getting remote root
    MElem* rroot = renv->GetNode("./renv_root"); 
    CPPUNIT_ASSERT_MESSAGE("Fail to get renv_root", rroot != 0);
    // Name
    string rr_name = rroot->Name(); 
    CPPUNIT_ASSERT_MESSAGE("Wrong name of rroot", rr_name == "renv_root");
    // Navigation
    MElem* l1node1 = rroot->GetNode("./renv_l1node1"); 
    CPPUNIT_ASSERT_MESSAGE("Fail to get renv_l1node1", l1node1 != NULL);
    string l1node1_name = l1node1->Name(); 
    CPPUNIT_ASSERT_MESSAGE("Wrong name of l1node1", l1node1_name == "renv_l1node1");
    // Gettng content
    string l1node1_cont;
    l1node1->GetCont(l1node1_cont);
    CPPUNIT_ASSERT_MESSAGE("Wrong content of l1node1", l1node1_cont == "Hello!");

    delete iEnv;
}


void ReadCspec(const string& aFileName, string& aCspec)
{
    ifstream ifs(aFileName.c_str());
    filebuf* pbuf = ifs.rdbuf();
    size_t size = pbuf->pubseekoff (0,ifs.end,ifs.in);
    pbuf->pubseekpos (0,ifs.in);
    char* buffer=new char[size];
    pbuf->sgetn(buffer,size);
    aCspec.insert(0, buffer, size);
    ifs.close();
    delete buffer;
}

/* Test suite to verify remote interaction to the primary model
 * The complete scheme is used: primary model is created remotelly, the model
 * in turn includes remote part. This part is created, root agent in the part
 * will be Renvu. It received link to primary env as a parameter on creation.
 * Renvu creates proxy to Renv agent in primary model, so bi-directional
 * interactions are supported that makes the model fully functional.
*/
class Ut_Bidir : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Bidir);
    CPPUNIT_TEST(test_Bidir_Cre);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Bidir_Cre();
    private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Bidir, "Ut_Bidir");


void Ut_Bidir::setUp()
{
}

void Ut_Bidir::tearDown() { }

// Test of creation simple model
void Ut_Bidir::test_Bidir_Cre()
{
    printf("\n === Test of Creating remote model with 2-dir communication\n");
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    // Creating model
    string cenv;
    bool res;
    // Checking env id
    string sid;
    res = client->Request("EnvProvider", "GetId", sid);
    printf("Getting session1 id -- Response: %s\n", sid.c_str());
    CPPUNIT_ASSERT_MESSAGE("Gettng session1 id failed: " + sid, res);
    // Create primary model env remotelly
    string cspec;
    ReadCspec("ut_bidir_cre.xml", cspec);
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + cspec, cenv);
    printf("Creating env: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating env failed: " + cenv, res);
    // Set SID to primary env
    string resp;
    res = client->Request(cenv, "SetEVar,1,SID," + sid, resp);
    printf("Setting Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Setting Session Id failed: " + resp, res);
    res = client->Request(cenv, "GetEVar,1,SID", resp);
    printf("Checking Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: " + resp, res);
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: incorrect value", resp == sid);
    // Construct primary model
    printf("Started creating model");
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Creating model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root failed: " + root, res);
    // Getting local native agent Elem
    string lelem;
    res = client->Request(root, "GetNode,1,Elem", lelem);
    printf("Getting local native agent Elem: %s\n", lelem.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local native agent Elem failed: " + lelem, res);
    // Getting local native agent Elem URI
    string lelem_uri;
    res = client->Request(lelem, "GetUri,1", lelem_uri);
    printf("Getting local native agent Elem URI: %s\n", lelem_uri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local native agent Elem URI failed: " + lelem_uri, res);
    // Getting local node v2
    string v2;
    res = client->Request(root, "GetNode,1,./v2", v2);
    printf("Getting local node v2: %s\n", v2.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local node v2 failed: " + v2, res);
    // Getting MVert local iface of local node v2
    string v2_vert;
    res = client->Request(v2, string("DoGetObj,1,") + MVert::Type(), v2_vert);
    printf("Getting MVert iface of local node v2: %s\n", v2_vert.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting MVert iface of local node v2 failed: " + v2_vert, res);
    // Getting local node Renv
    string renv;
    res = client->Request(root, "GetNode,1,./Renv", renv);
    printf("Getting local node Renv: %s\n", renv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local node Renv failed: " + renv, res);
    // Getting remote root
    string rroot;
    res = client->Request(root, "GetNode,1,./Renv/renv_root", rroot);
    printf("Getting remote root: %s\n", rroot.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root failed: " + rroot, res);
    // Getting remote root name
    res = client->Request(rroot, "Name", resp);
    printf("Getting remote root name: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root name failed: " + resp, res);
    // Getting remote root owner
    res = client->Request(rroot, "GetMan", resp);
    printf("Getting remote root owner: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root owner failed: " + resp, res);
    // Getting remote root absolute URI
    string rroot_uri;
    res = client->Request(rroot, "GetUri,1", rroot_uri);
    printf("Getting remote root absolute URI: %s\n", rroot_uri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root absolute URI failed: " + rroot_uri, res);
    // Getting upper node via remote root
    res = client->Request(rroot, "GetNode,1,/*", resp);
    printf("Getting root via remote root: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root via remote root failed: " + resp, res);
    // Getting remote root comps count
    res = client->Request(rroot, "CompsCount,1", resp);
    printf("Getting remote root comps count: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root comps count failed: " + resp, res && resp == "2");
    // Getting remote root compa#0
    res = client->Request(rroot, "GetComp,1,0", resp);
    printf("Getting remote root comp#0: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root comp#0: " + resp, res);
    // Getting remote root compa#1
    res = client->Request(rroot, "GetComp,1,1", resp);
    printf("Getting remote root comp#1: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root comp#1: " + resp, res);
    // Getting remote remote_node_1
    string rnode1;
    res = client->Request(root, "GetNode,1,./Renv/renv_root/remote_node_1", rnode1);
    printf("Getting remote remote_node_1: %s\n", rnode1.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote remote_node_1 failed: " + rnode1, res);
    // Adding new node remote_node_3 to renv_root
    string req = Ifu::CombineIcSpec("AppendMutation", "1", "node,id:remote_node_3,parent:Elem");
    res = client->Request(rroot, req, resp);
    printf("Adding new node remote_node_3 to renv_root, setting mut: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Adding new node remote_node_3 to renv_root, setting mut: " + resp, res);
    res = client->Request(rroot, "Mutate#2,1,false,true,true", resp);
    printf("Adding new node remote_node_3 to renv_root, mutation: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Adding new node remote_node_3 to renv_root, mutation: " + resp, res);
    // Checking new node addition
    string rnode3;
    res = client->Request(root, "GetNode,1,./Renv/renv_root/remote_node_3", rnode3);
    printf("Getting remote remote_node_3: %s\n", rnode3.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote remote_node_3 failed: " + rnode3, res);
    // Getting URI of remote remote_node_1
    string rnode1_uri;
    res = client->Request(rnode1, "GetUri,1", rnode1_uri);
    printf("Getting URI of remote remote_node_1: %s\n", rnode1_uri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting URI of remote remote_node_1 failed: " + rnode1_uri, res);
    // Getting local iface of remote remote_node_1
    string rnode1_vert;
    res = client->Request(rnode1, string("DoGetObj,1,") + MVert::Type(), rnode1_vert);
    printf("Getting MVert iface of remote remote_node_1: %s\n", rnode1_vert.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting MVert iface of remote remote_node_1 failed: " + rnode1_vert, res);
    // Getting remote_node_1 URI relative to remote root
    string rnode1_ruri;
    res = client->Request(rnode1, "GetUri#2,1," + rroot_uri, rnode1_ruri);
    printf("Getting remote_node_1 URI relative to remote root: %s\n", rnode1_ruri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root URI relative to remote_node_1 failed: " + rnode1_ruri, res && rnode1_ruri == "./remote_node_1");
    // Getting remote remote_node_2 created from remote parent (from primary env)
    string rnode2;
    res = client->Request(root, "GetNode,1,./Renv/renv_root/remote_node_2", rnode2);
    printf("Getting remote remote_node_2: %s\n", rnode2.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote remote_node_2 failed: " + rnode2, res);
    // Getting remote native agent Elem
    string relem;
    res = client->Request(rroot, "GetNode,1,Elem", relem);
    printf("Getting remote native agent Elem: %s\n", relem.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote native agent Elem failed: " + relem, res);
    // Getting remote native agent Elem URI
    string relem_uri;
    res = client->Request(relem, "GetUri,1", relem_uri);
    printf("Getting remote native agent Elem URI: %s\n", relem_uri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote native agent Elem URI failed: " + relem_uri, res);
    // Getting remote native node URI relative to remote root 
    string relem_ruri;
    res = client->Request(relem, "GetUri#2,1," + rroot_uri, relem_ruri);
    printf("Getting remote native node URI relative to remote root: %s\n", relem_ruri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote native node URI relative to remote root failed: " + relem_ruri, res && relem_ruri == "Elem");

    client->Disconnect();
    delete client;
}


/* Test suite to verify Vertex-level remote interaction to the primary model
*/
class Ut_Vert : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Vert);
    CPPUNIT_TEST(test_Vert_Cre);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Vert_Cre();
    private:
    Env* iEnv;
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Vert, "Ut_Vert");


void Ut_Vert::setUp()
{
}

void Ut_Vert::tearDown() { }

// Test of creation Vertex based  model
void Ut_Vert::test_Vert_Cre()
{
    printf("\n === Test of Creating remote Vertex model with 2-dir communication\n");
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    // Creating model
    string cenv;
    bool res;
    // Checking env id
    string sid;
    res = client->Request("EnvProvider", "GetId", sid);
    printf("Getting session1 id -- Response: %s\n", sid.c_str());
    CPPUNIT_ASSERT_MESSAGE("Gettng session1 id failed: " + sid, res);
    // Create primary model env remotelly
    string cspec;
    ReadCspec("ut_vert_cre.xml", cspec);
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + cspec, cenv);
    printf("Creating env: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating env failed: " + cenv, res);
    // Set SID to primary env
    string resp;
    res = client->Request(cenv, "SetEVar,1,SID," + sid, resp);
    printf("Setting Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Setting Session Id failed: " + resp, res);
    res = client->Request(cenv, "GetEVar,1,SID", resp);
    printf("Checking Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: " + resp, res);
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: incorrect value", resp == sid);
    // Construct primary model
    printf("Started creating model");
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Creating model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root failed: " + root, res);
    // Getting local node v2
    string v2;
    res = client->Request(root, "GetNode,1,./v2", v2);
    printf("Getting local node v2: %s\n", v2.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local node v2 failed: " + v2, res);
    // Getting MVert local iface of local node v2
    string v2_vert;
    res = client->Request(v2, string("DoGetObj,1,") + MVert::Type(), v2_vert);
    printf("Getting MVert iface of local node v2: %s\n", v2_vert.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting MVert iface of local node v2 failed: " + v2_vert, res);
    // Getting iface range from v2
    string v2_ifcr;
    res = client->Request(v2, "GetIfi,1,MVert,", v2_ifcr);
    printf("Getting iface range from local node v2: %s\n", v2_ifcr.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface range from local node v2 failed: " + v2_ifcr, res && v2_ifcr == "1");
    // Getting iface from v2
    string v2_ifind;
    res = client->Request(v2, "GetIfind,1,MVert,,0", v2_ifind);
    printf("Getting iface from local node v2: %s\n", v2_ifind.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface from local node v2 failed: " + v2_ifind, res);
    // Getting local node Renv
    string renv;
    res = client->Request(root, "GetNode,1,./Renv", renv);
    printf("Getting local node Renv: %s\n", renv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local node Renv failed: " + renv, res);
    // Getting remote root
    string rroot;
    res = client->Request(root, "GetNode,1,./Renv/renv_root", rroot);
    printf("Getting remote root: %s\n", rroot.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root failed: " + rroot, res);
    // Getting remote root name
    res = client->Request(rroot, "Name", resp);
    printf("Getting remote root name: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root name failed: " + resp, res);
    // Getting remote root owner
    res = client->Request(rroot, "GetMan", resp);
    printf("Getting remote root owner: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote root owner failed: " + resp, res);
    // Getting upper node via remote root
    res = client->Request(rroot, "GetNode,1,/*", resp);
    printf("Getting root via remote root: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root via remote root failed: " + resp, res);
    // Getting remote remote_node_1
    string rnode1;
    res = client->Request(root, "GetNode,1,./Renv/renv_root/remote_node_1", rnode1);
    printf("Getting remote remote_node_1: %s\n", rnode1.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote remote_node_1 failed: " + rnode1, res);
    // Getting URI of remote remote_node_1
    string rnode1_uri;
    res = client->Request(rnode1, "GetUri,1", rnode1_uri);
    printf("Getting URI of remote remote_node_1: %s\n", rnode1_uri.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting URI of remote remote_node_1 failed: " + rnode1_uri, res);
    // Getting local iface of remote remote_node_1
    string rnode1_vert;
    res = client->Request(rnode1, string("DoGetObj,1,") + MVert::Type(), rnode1_vert);
    printf("Getting MVert iface of remote remote_node_1: %s\n", rnode1_vert.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting MVert iface of remote remote_node_1 failed: " + rnode1_vert, res);
    // Getting iface range from proxy of remote_node_1
    string rnode1_ifcr;
    res = client->Request(rnode1, "GetIfi,1,MVert,", rnode1_ifcr);
    printf("Getting iface range from proxy of  remote_node_1: %s\n", rnode1_ifcr.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface range from proxy of remote_node_1: " + rnode1_ifcr, res && rnode1_ifcr == "1");
    // Getting iface from proxy of remote_node_1
    string rnode1_ifind;
    res = client->Request(rnode1, "GetIfind,1,MVert,,0", rnode1_ifind);
    printf("Getting iface from proxy of remote_node_1: %s\n", rnode1_ifind.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface from proxy of remote_node_1: " + rnode1_ifind, res);
    // Getting remote remote_node_2 created from remote parent (from primary env)
    string rnode2;
    res = client->Request(root, "GetNode,1,./Renv/renv_root/remote_node_2", rnode2);
    printf("Getting remote remote_node_2: %s\n", rnode2.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting remote remote_node_2 failed: " + rnode2, res);
    // Getting primary env vertex v1
    string v1;
    res = client->Request(root, "GetNode,1,./v1", v1);
    printf("Getting primary env vertex v1: %s\n", v1.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting primary env vertex v1 failed: " + v1, res);
    // Getting vertex iface of primary env vertex v1
    string v1_vert;
    res = client->Request(v1, string("DoGetObj,1,") + MVert::Type(), v1_vert);
    printf("Getting vertex iface of primary env vertex v1: %s\n", v1_vert.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting vertex iface of primary env vertex v1 failed: " + v1_vert, res);
    // Getting count of pairs of primary env vertex v1
    string v1_pairs_cnt;
    res = client->Request(v1, "PairsCount,1", v1_pairs_cnt);
    printf("Getting pairs count of primary env vertex v1: %s\n", v1_pairs_cnt.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting pairs count of primary env vertex v1 failed", v1_pairs_cnt == "1");
    // Getting pair of primary env vertex v1
    string v1_pair;
    res = client->Request(v1, "GetPair,1,0", v1_pair);
    printf("Getting pair of primary env vertex v1: %s\n", v1_pair.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting pair of primary env vertex v1 failed", v1_pair != "[NONE]");

    client->Disconnect();
    delete client;
}



/* Test suite to verify Syst-level remote interaction to the primary model
*/
class Ut_Syst : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Syst);
    CPPUNIT_TEST(test_Syst_Cre);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_Syst_Cre();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Syst, "Ut_Syst");


void Ut_Syst::setUp() {}

void Ut_Syst::tearDown() {}

// Test of creation Syst based  model
void Ut_Syst::test_Syst_Cre()
{
    printf("\n === Test of Creating remote Syst model with 2-dir communication\n");
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    // Creating model
    string cenv;
    bool res;
    // Checking env id
    string sid;
    res = client->Request("EnvProvider", "GetId", sid);
    printf("Getting session1 id -- Response: %s\n", sid.c_str());
    CPPUNIT_ASSERT_MESSAGE("Gettng session1 id failed: " + sid, res);
    // Create primary model env remotelly
    string cspec;
    ReadCspec("ut_syst_cre.xml", cspec);
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + cspec, cenv);
    printf("Creating env: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating env failed: " + cenv, res);
    // Set SID to primary env
    string resp;
    res = client->Request(cenv, "SetEVar,1,SID," + sid, resp);
    printf("Setting Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Setting Session Id failed: " + resp, res);
    res = client->Request(cenv, "GetEVar,1,SID", resp);
    printf("Checking Session Id: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: " + resp, res);
    CPPUNIT_ASSERT_MESSAGE("Checking Session Id failed: incorrect value", resp == sid);
    // Construct primary model
    printf("Started creating model");
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Creating model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root failed: " + root, res);
    // Getting local node conn point Syst1/Cp1
    string cp1;
    res = client->Request(root, "GetNode,1,./Syst1/Cp1", cp1);
    printf("Getting local conn point Syst1/Cp1: %s\n", cp1.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting local conn point Syst1/Cp1 failed: " + cp1, res);
    // Getting iface range from local conn point Syst1/Cp1
    string cp1_ifcr;
    res = client->Request(cp1, "GetIfi,1,MProp,", cp1_ifcr);
    printf("Getting iface range from local conn point Syst1/Cp1: %s\n", cp1_ifcr.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface range from conn point Syst1/Cp1 failed: " + cp1_ifcr, res && cp1_ifcr == "1");
    // Getting iface from local conn point Syst1/Cp1
    string cp1_ifind;
    res = client->Request(cp1, "GetIfind,1,MProp,,0", cp1_ifind);
    printf("Getting iface from local conn point Syst1/Cp1: %s\n", cp1_ifind.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting iface from conn point Syst1/Cp1 failed: " + cp1_ifind, res);
    // Getting value from obtained MProp iface
    string token_val;
    res = client->Request(cp1_ifind, "Value", token_val);
    printf("Getting value from obtained MProp iface: %s\n", token_val.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting value from obtained MProp iface failed: " + token_val, res);


    client->Disconnect();
    delete client;
}



/* Test suite to verify agents observer support
*/
class Ut_Obs : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_Obs);
    CPPUNIT_TEST(test_Obs_Cre);
    CPPUNIT_TEST_SUITE_END();
    public:
    class Ut_Obs_Ago: public MAgentObserver
    {
	public:
	    Ut_Obs_Ago();
	    virtual ~Ut_Obs_Ago();
	public:
	    virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	    virtual void OnCompAdding(MElem& aComp);
	    virtual TBool OnCompChanged(MElem& aComp);
	    virtual TBool OnContentChanged(MElem& aComp);
	    virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	    // From MIface	
	    virtual MIface* Call(const string& aSpec, string& aRes);
	    virtual string Mid() const;
    };
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    static void* RunBackSrv(void *args);
    private:
    void test_Obs_Cre();
    private:
    Server* mBSrv;
    Ut_Obs_Ago* mAgo;
    pthread_t mTid;
    static const int BSRV_PORT;
    static const string BSRV_PORT_S;
};

const int Ut_Obs::BSRV_PORT = 30678;
const string Ut_Obs::BSRV_PORT_S = "30678";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_Obs, "Ut_Obs");

void Ut_Obs::Ut_Obs_Ago::OnCompDeleting(MElem& aComp, TBool aSoft)
{
}

void Ut_Obs::Ut_Obs_Ago::OnCompAdding(MElem& aComp)
{
}

TBool Ut_Obs::Ut_Obs_Ago::OnCompChanged(MElem& aComp)
{
    return true;
}

TBool Ut_Obs::Ut_Obs_Ago::OnContentChanged(MElem& aComp)
{
    return true;
}

TBool Ut_Obs::Ut_Obs_Ago::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    return true;
}

Ut_Obs::Ut_Obs_Ago::Ut_Obs_Ago()
{
}


Ut_Obs::Ut_Obs_Ago::~Ut_Obs_Ago()
{
}

string Ut_Obs::Ut_Obs_Ago::Mid() const
{
    return string();
}

MIface* Ut_Obs::Ut_Obs_Ago::Call(const string& aSpec, string& aRes)
{
    MIface* res = NULL;
    string name, sig;
    vector<string> args;
    Ifu::ParseIcSpec(aSpec, name, sig, args);
    TBool name_ok = mIfu.CheckMname(name);
    if (!name_ok) {
	CPPUNIT_ASSERT_MESSAGE("Wrong method name", false);
    }
    TBool args_ok = mIfu.CheckMpars(name, args.size());
    if (!args_ok) {
	CPPUNIT_ASSERT_MESSAGE("Wrong arguments number", false);
    }
    if (name == "OnCompDeleting") {
	cout << "OnCompDeleting: " << args.at(0) << endl;
    } else if (name == "OnCompAdding") {
	cout << "[AgentObserver] OnCompAdding: " << args.at(0) << endl;
    } else {
	CPPUNIT_ASSERT_MESSAGE("Unhandled method", false);
    }
    return res;
}


// static
void* Ut_Obs::RunBackSrv(void *args)
{
    Ut_Obs* self = (Ut_Obs*) args;
    self->mBSrv->AcceptAndDispatch();
    return NULL;
}

void Ut_Obs::setUp()
{
    mBSrv = new Server(BSRV_PORT);
    mAgo = new Ut_Obs_Ago();
}

void Ut_Obs::tearDown()
{
    delete mAgo;
    delete mBSrv;
}

// Test of creation model and connecting to observer
void Ut_Obs::test_Obs_Cre()
{
    printf("\n === Test of Creating remote model and connecting agent observer\n");
    // Create back server
    int th = pthread_create(&this->mTid, NULL, (void *(*)(void *))RunBackSrv, this);
    // Add Agent Observer to server resources
    CSessionBase::AddSContext("AObs_1", mAgo);
    // Create client session 
    BaseClient* client = new BaseClient();
    // Wait until server run
    bool srv_run = WaitSrv();
    CPPUNIT_ASSERT_MESSAGE("Server isn't running", srv_run);
    try {
	client->Connect("");
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Error connecting to server", false);
    }
    printf("Client connected to the server\n");
    // Creating model
    string cenv;
    bool res;
    // Checking env id
    string sid;
    res = client->Request("EnvProvider", "GetId", sid);
    printf("Getting session1 id -- Response: %s\n", sid.c_str());
    CPPUNIT_ASSERT_MESSAGE("Gettng session1 id failed: " + sid, res);
    // Create primary model env remotelly
    string cspec;
    string resp;
    ReadCspec("ut_obs_cre.xml", cspec);
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + cspec, cenv);
    printf("Creating env: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating env failed: " + cenv, res);
    // Construct primary model
    printf("Started creating model");
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Creating model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root failed: " + root, res);
    // Requesting to connect observer
    string aobs_uri = "socks://localhost:" + BSRV_PORT_S + "/AObs_1#"; 
    res = client->Request(root, "SetObserver,1," + aobs_uri, resp);
    printf("Connecting observer: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Connecting observer failed: " + resp, res);
    // Adding new component to the root
    res = client->Request(root, "Mutate,1,<node><node id=\"node_1\" parent=\"Elem\"></node></node>,false,true,true", resp);
    printf("Adding new component to the root: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Adding new component to the root failed: " + resp, res);
    delete client;
}

