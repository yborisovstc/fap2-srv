#include <iostream>
#include "../server/sserver.h"

using namespace std;

int main() {
    cout << "Running grayb server!" << endl;
    Server *s;
    s = new Server();
    //Main loop
    s->AcceptAndDispatch();
    return 0;
}
