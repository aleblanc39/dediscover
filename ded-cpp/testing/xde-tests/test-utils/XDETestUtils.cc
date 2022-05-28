/**
 * General utility functions for use by the different test programs.
 */

#include <TestUtils.h>
#include <boost/cast.hpp>

#include <base/XDEBlas.h>

using namespace std;

void displayErrorsCount(string pgmName, int nbErrors) {
    if (!nbErrors)
        std::cerr << pgmName << " completed successfully.\n";
    else
        std::cerr << pgmName << " completed with " << nbErrors << " error"
                  << ((nbErrors == 1) ? "" : "s") << "\n";
}


void getControlPoints(TDoubleVector &x, TDoubleVector &y) {
    double xx[] = {-1, 0, 1, 3, 4, 5, 8, 9};
    double yy[] = {-8, -3.4, 5.9, 6.7, 3.3, 11, 10};
    unsigned size = sizeof(xx) / sizeof(double);

    x.resize(size);
    y.resize(size);

    copy(xx, xx + size - 1, x.begin());
    copy(yy, yy + size - 1, y.begin());
}



ModelPtr getBasicModel() {
    return ModelPtr(new XDEModel("dx/dt=1\nMAC=x/2\n"));
}


void reportError(int no, string progName, string procName, string msg) {
    string ret = "Error " + boost::lexical_cast<string>(no) + " in " +
                 progName + "::" + procName + "\n";
    if (msg != "") ret.append("Message: " + msg).append("\n");
    std::cerr << ret;
}

