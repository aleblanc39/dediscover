#include <TestUtils.h>

#include <base/XDEBlas.h>
#include <extra_parser_functions/PrecomputedSpline.h>
using namespace std;

/**
 * Program to test the functionality of the MappingInfo and
 * mappingInfoSet classes.
 */


static void testDuplicateTimepoints();
static void testValidCase();
static int nbErrors = 0;
static char *pgmName;


int main(int, char **argv) {
    pgmName = argv[0];

    testDuplicateTimepoints();
    testValidCase();
    displayErrorsCount(argv[0],  nbErrors);
}


void testDuplicateTimepoints() {
    cerr << "Testing initialization\n";
    double times [] = {0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,5.5,5.5,5.5,6,6,6.5,6.5,6.5,7,7,7,7.5,7.5,7.5,8,8,8,9,9,9,9.5,10,10,10,10.5,10.5,10.5,11,11,11,11.5,12,12,12,14,14,14,16,16,16,20,20,20,28,28,28,35,35,35,84,84,84};

    double values [] = {45,17,-57,-10,-7,1,55,-71,-37,-82,156,-59,-15,21,-241,238,-299,115,1033,709,959,20183,5326,12483,11836,5822,3051,2217,280,11363,11031,13200,67864,37826,42343,58041,66990,52066,30550,11409,18405,21718,10710,6309,20061,20998,20952,12180,9217,6627,7263,25362,4027,4931,5322,1758,2174,1620,1640,2691,1594,459,91,273,178,356,352,68,-22,-8};

    double derivatives[] = {45,17,-57,-10,-7,1,55,-71,-37,-82,156,-59,-15,21,-241,238,-299,115,1033,709,959,20183,5326,12483,11836,5822,3051,2217,280,11363,11031,13200,67864,37826,42343,58041,66990,52066,30550,11409,18405,21718,10710,6309,20061,20998,20952,12180,9217,6627,7263,25362,4027,4931,5322,1758,2174,1620,1640,2691,1594,459,91,273,178,356,352,68,-22,-8};

    TDoubleVector vtimes(70);
    TDoubleVector vvalues(70);
    TDoubleVector vderivatives(70);

    for (unsigned i = 0; i < 70; i++) {
	vtimes[i] = times[i];
	vvalues[i] = values[i];
	vderivatives[i] = derivatives[i];
    }


    try {
	DerivableSplinePtr spline(new PrecomputedSpline(vtimes, vvalues, vderivatives));
	spline -> initialize();
	// Should not make it here
	cerr << "Error 1 in " << pgmName << ": Exception was not thrown\n";
	nbErrors++;
	
    } catch (XDEException &e) {
	cerr << "Got exception : " << e.getMsg() << " ---- ok\n";
    }
}

void testValidCase() {

    double times [] = {0, 2.44, 5.36, -1, 3, 6};

    double values [] = {2, 4, 6, 8, 10, 12};

    double derivatives[] = {1,3,5,7,9,11};

    unsigned nbValues = sizeof(times)/sizeof(double);

    TDoubleVector vtimes(nbValues);
    TDoubleVector vvalues(nbValues);
    TDoubleVector vderivatives(nbValues);

    copy(times, times + nbValues, vtimes.begin());
    copy(values, values + nbValues, vvalues.begin());
    copy(derivatives, derivatives + nbValues, vderivatives.begin());

    try {
	DerivableSplinePtr spline(new PrecomputedSpline(vtimes, vvalues, vderivatives));
	spline -> initialize();

	// Query a few values.

	// First one that should throw an exception
	try {
	    double d = spline -> compute(2.54);
	    cerr << "Error 2 in " << pgmName << ": did not catch the exception\n";
	    cerr << "Returned value: " << d << endl;
	    nbErrors++;
	} catch (XDEException e) {
	    cerr << "Got exception: " << e.getMsg() << " ... ok\n";
	}

	// Test one of the values
	if (!double_equals(spline -> compute(2*1.22), 4)) {
	    cerr << "Error 3 in " << pgmName << " expexting 4, returned " 
		 << spline -> compute(2*1.22) << endl;
	    nbErrors++;
	}

	// Then test one of the derivatives.
	if (!double_equals(spline -> computeDerivative(0-1), 7)) {
	    cerr << "Error 4 in " << pgmName << " expexting 7, returned " 
		 << spline -> computeDerivative(0-1) << endl;
	    nbErrors++;
	}

	


	
    } catch (...) {
	cerr << "Error 2 in " << pgmName << "Got exception : \n";
	nbErrors++;
    }
}


