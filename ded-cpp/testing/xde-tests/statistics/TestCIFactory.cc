#include <iostream>

#include <TestUtils.h>
#include <statistics/CIFactory.h>

using namespace std;
static CIFactory ciFactory;

int main(int argc, char **argv) {
    XDEMessage::setLogLevel(XDEMessage::WARNING);
    cerr << "Should have created a CI Factory" << endl;
    
}