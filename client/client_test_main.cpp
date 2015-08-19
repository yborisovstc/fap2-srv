#include <iostream>
#include "bclient.h"

using namespace std;

int main() {
    cout << "Running fap2 test client!" << endl;
    BaseClient *c;
    c = new BaseClient();
    //Main loop
    c->Dispatch();
    return 0;
}
