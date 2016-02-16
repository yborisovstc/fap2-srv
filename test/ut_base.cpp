#include <stdlib.h>
#include "../client/bclient.h"
#include "../server/requests.h"
#include <sys/types.h>
#include <signal.h>
#include <env.h>
#include <elem.h>
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
	string resp;
	RequestIPC ipc;
	// Request -get_models_list-
	try {
	    client->Request(RequestIPC::REQ_GET_MODELS_LIST, resp);
	} catch (exception& e) {
	    CPPUNIT_ASSERT_MESSAGE("Request -get_models_list- failed", false);
	}
	printf("Response: %s\n", resp.c_str());
	// Kill server process
	//kill(serv_pid, -9);
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
 */
void Ut_Renva::test_Renva_Cre()
{
    printf("\n === Test of Creating remote env agent\n");
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
    RequestIPC ipc;
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
    ifstream ifs("ut_bidir_cre.xml");
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
    res = client->Request(cenv, "ConstructSystem", resp);
    printf("Creating model: %s\n", resp.c_str());
    CPPUNIT_ASSERT_MESSAGE("Creating model failed: " + resp, res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Getting root failed: " + root, res);
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

    client->Disconnect();
    delete client;
}

