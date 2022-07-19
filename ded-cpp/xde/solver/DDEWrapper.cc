#include "solver/DDEWrapper.h"

using namespace std;


vector<string> DDEWrapper::s_getMethodAttributes() {
    vector<string> attrs = XDESolverWrapper::getAttributes();
    attrs.push_back("DDE");
    return attrs;
}
