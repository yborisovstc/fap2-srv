#include <stdlib.h>
#include "../client/bclient.h"
#include "../server/requests.h"
#include <sys/types.h>
#include <signal.h>
#include <env.h>
#include <elem.h>
#include <daaprov.h>

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
    // Issue request for creating model
    try {
	client->Request("EnvProvider", KMeth_CreateEnv + ",1," + KChromo_1, resp);
	printf("Create model -- Response: %s\n", resp.c_str());
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Request -create_model- failed", false);
    }
    // Issue request for root
    try {
	client->Request(resp, "Root", resp);
	printf("Getting root -- Response: %s\n", resp.c_str());
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Request -get root- failed", false);
    }
    // Issue request for root name
    try {
	client->Request(resp, "Name", resp);
	printf("Getting root name -- Response: %s\n", resp.c_str());
    } catch (exception& e) {
	CPPUNIT_ASSERT_MESSAGE("Request -get root name- failed", false);
    }
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
    // Creating model
    string cenv;
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + KChromo_1, cenv);
    printf("Create model -- Response: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -create_model- failed", res);
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

// Test of request "Exec Melem iface methods"
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
    CPPUNIT_ASSERT_MESSAGE("Request -GetId- failed", res);
    // Create primary model remotelly
    res = client->Request("EnvProvider", KMeth_CreateEnv + ",1," + KChromo_1, cenv);
    printf("Create model -- Response: %s\n", cenv.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -create_model- failed", res);
    // Getting root
    string root;
    res = client->Request(cenv, "Root", root);
    printf("Getting root -- Response: %s\n", root.c_str());
    CPPUNIT_ASSERT_MESSAGE("Request -get root- failed", res);

    client->Disconnect();
    delete client;
}

