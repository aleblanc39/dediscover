#include <TestUtils.h>
#include <extra_parser_functions/LOCF.h>

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>

using namespace boost::assign;

using namespace std;

/**
 * Program to test the functionality of the MappingInfo and
 * mappingInfoSet classes.
 */

static int nbErrors = 0;

void testLOCFInModel() {
    string eqns =
        "dx/dt=1\n\
    X=locf(t,[1,2,3,4],[9,8,7,6])\n";
    // X=x+1\n";

    XDEModel model(eqns);

    vector<double> timepoints;  // = boost::assign::list_of(0.0) (1.0) (2.0)
                                // (3.0) (4.0).to_adapter();
    timepoints += 0.0, 1.0, 2.0, 3,
        4;  //= boost::assign::list_of(0.0) (1.0) (2.0) (3.0) (4.0);
    TDoubleVector vtimepoints(timepoints.size());
    copy(timepoints.begin(), timepoints.end(), vtimepoints.begin());

    cerr << "timepoints: " << vtimepoints << endl;
    TDoubleMatrix depVarValues(timepoints.size(), 1);
    depVarValues(0, 0) = 2.5;

    TDoubleMatrix macroResults =
        model.computeMacrosAtTimepoints(vtimepoints, depVarValues);
    cerr << "macroResults: " << macroResults << endl;
}

void testLOCF() {
    LOCF locf;

    vector<double> timepoints;  // = boost::assign::list_of(0.0) (1.0) (2.0)
                                // (3.0) (4.0).to_adapter();
    timepoints += 0.0, 1.0, 2.0, 3, 4;  
    // timepoints= boost::assign::list_of(0.0) (1.0) (2.0) (3.0) (4.0);
    TDoubleVector vtimepoints(timepoints.size());
    copy(timepoints.begin(), timepoints.end(), vtimepoints.begin());

    vector<double> values;  // = boost::assign::list_of(0.0) (1.0) (2.0) (3.0)
                            // (4.0).to_adapter();
    values += 0.5, 1.8, 0.2, 3.5,
        4.7;  //= boost::assign::list_of(0.0) (1.0) (2.0) (3.0) (4.0);
    TDoubleVector vvalues(values.size());
    copy(values.begin(), values.end(), vvalues.begin());

    locf.addControlPoints(vtimepoints, vvalues);
    locf.initialize();

    for (double d = 0; d < 10; d += 0.4) {
        cerr << d << ": " << locf.compute(d) << endl;
    }
}

int main(int, char **argv) {
    //testLOCFInModel();
    testLOCF();
    displayErrorsCount(argv[0], nbErrors);
}
