#include <TestUtils.h>
#include <iostream>


#include <extra_parser_functions/FortranSmoothingBSpline.h>
using namespace std;

/**
 * Program to test the functionality of the MappingInfo and
 * mappingInfoSet classes.
 */


static int nbErrors = 0;


void testSpline() {


    double times [] = {0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,5.5,5.5,5.5,6,6,6.5,6.5,6.5,7,7,7,7.5,7.5,7.5,8,8,8,9,9,9,9.5,10,10,10,10.5,10.5,10.5,11,11,11,11.5,12,12,12,14,14,14,16,16,16,20,20,20,28,28,28,35,35,35,84,84,84};

    double values [] = {45,17,-57,-10,-7,1,55,-71,-37,-82,156,-59,-15,21,-241,238,-299,115,1033,709,959,20183,5326,12483,11836,5822,3051,2217,280,11363,11031,13200,67864,37826,42343,58041,66990,52066,30550,11409,18405,21718,10710,6309,20061,20998,20952,12180,9217,6627,7263,25362,4027,4931,5322,1758,2174,1620,1640,2691,1594,459,91,273,178,356,352,68,-22,-8};

    unsigned size = sizeof(times)/sizeof(double);

    FSmoothingBSpline spline;
    for (unsigned i = 0; i < size; i++) {
	spline.addControlPoint(times[i], values[i]);
    }

    spline.initialize();
    cerr << "Computed spline values\n";
    for (double t = 0; t < 84; t+= 1) {
	cerr << t << ": " << spline.compute(t) << endl;
    }
}

static void testPatriceModel() {
    TDoubleVector tout;
    TDoubleMatrix yout;

    CBIMSessionFileV1_2 sessionFile(getSessionInputFileV1_3("patrice_spline_test"));
    SimulationParameterSetPtr ps = getSimulationParameterSet(sessionFile);
    SolverPtr solver = sessionFile.getSolver();
    ModelPtr model = sessionFile.getModel();
    
    solver -> solve(model, *(ps.get()), tout, yout);
    cerr << "Values for tout, yout: " << tout << endl << yout << endl;
}


int main(int, char **argv) {
    testSpline();
    testPatriceModel();
    displayErrorsCount(argv[0],  nbErrors);
}


