/**
 * Basic program to test calling the fortran subroutine from C++. The
 * subroutine will be located in a shared library.
 */

#include <iostream>
#include <fstream>

using namespace std;

typedef struct  {
    double *knots;
    double *coeffs;
    int order;
} sp;

extern "C" {
    void c_spaps(double *x,
		 double *y,
		 double tol,
		 double *wi,
		 int order,
		 FSmoothingBSpline::sp *spline,
		 int size,
		 int autoweight,
		 int usegcv);

    double c_bsp_eval(FSmoothingBSpline::sp *spline,
		      double *x);
}



static void testSmall() {
    int size = 21;
    double x[size];
    double y[size];
    double tol = -0.5;
    double wi[size];

    sp spline;
    
    ifstream infile;
    infile.open("data.txt");

    for (unsigned i = 0; i < size; i++) {
	infile >> x[i];
	cerr << "Read: " << x[i] << endl;
    }

    for (unsigned i = 0; i < size; i++) {
	infile >> y[i];
	cerr << "Read: " << y[i] << endl;
    }

    for (unsigned i = 0; i < size; i++) {
	wi[i] = 1.0;
    }

    // Looks like both tol and wi are optional arguments, and only 
    cerr << "Calling spline\n";
    //spline_mp_spaps_(&x[0], y, &tol, wi, &spline, &size);
    c_spaps(&x[0], y, &tol, wi, &spline, &size);
    cerr << "Spline created. Compute spline values\n";
    for ( double i = 0; i < x[size-1]; i+= 0.1) {
	//cerr << i << ": " << spline_mp_bsp_eval_(&spline, &i) << endl;
    }
}


static void testBig() {
    int size = 841;
    double x[size];
    double y[size];
    double tol = -0.5;
    double wi[size];

    sp spline;
    
    ifstream infile;
    infile.open("big-data.csv");

    for (unsigned i = 0; i < size; i++) {
	infile >> x[i];
	cerr << "Read: " << x[i] << endl;
    }

    for (unsigned i = 0; i < size; i++) {
	infile >> y[i];
	cerr << "Read: " << y[i] << endl;
    }

    for (unsigned i = 0; i < size; i++) {
	wi[i] = 1.0;
    }

    // Looks like both tol and wi are optional arguments, and only 
    cerr << "Calling spline\n";
    //spline_mp_spaps_(&x[0], y, &tol, wi, &spline, &size);
    cerr << "Creating big spline\n";
    c_spaps(&x[0], y, &tol, wi, &spline, &size);
    cerr << "Spline created. Compute spline values\n";
    for ( double i = 0; i < x[size-1]; i+= 0.1) {
	//cerr << i << ": " << spline_mp_bsp_eval_(&spline, &i) << endl;
    }
}



main (int, char **) {
    testSmall();
    testBig();
}
