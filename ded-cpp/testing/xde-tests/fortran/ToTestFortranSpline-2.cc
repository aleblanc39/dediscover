/**
 * Basic program to test calling the fortran subroutine from C++. The
 * subroutine will be located in a shared library.
 */

#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

typedef struct  {
    double *knots;
    double *coefs;
    int order,sizeknots,sizecoefs;
} sp;


extern "C" {void c_spaps(double *x,
			 double *y,
			 double tol,
			 double *wi,
			 int order,
			 sp *spline,
			 int size,
			 int autowwights,
			 int usegcv);

    double c_bsp_eval(sp *spline,
		      double *x);

}


static void testSmall() {
    int size = 21;
    double *x,*y,*wi;
    x=(double*)malloc(size*sizeof(double));
    y=(double*)malloc(size*sizeof(double));
    wi=(double*)malloc(size*sizeof(double));
    double tol = -0.5;

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
    c_spaps(x,y,tol,wi,2,&spline,size,1,0);
    cout<<"In C spline order"<<spline.order<<endl;
    cout<<"In C spline szk"<<spline.sizecoefs<<endl;
    cout<<"In C spline szcoefs"<<spline.sizeknots<<endl;
    for (unsigned i = 0; i < spline.sizeknots; i++) {
	    cout<<"In C spline knots "<<spline.knots[i]<<endl;
    }

    for (unsigned i = 0; i < spline.sizecoefs; i++) {
	    cout<<"In C spline coefs "<<spline.coefs[i]<<endl;
    }
    //cin.get();
    cerr << "Spline created. Compute spline values\n";
    for ( double i = 0; i < x[size-1]; i+= 0.1) {
	    //c_spaps(x,y,tol,wi,&spline,size);
	    cout << i << ": " << c_bsp_eval(&spline, &i) << endl;
	    //free(spline.knots);
	    //free(spline.coefs);

    }

    delete[] x;
    delete [] y;
    delete []wi;
}


static void testBig() {
    int size = 841;
    double *x,*y,*wi;
    x=(double*)malloc(size*sizeof(double));
    y=(double*)malloc(size*sizeof(double));
    wi=(double*)malloc(size*sizeof(double));
    double tol = -8.5;

    sp spline;
    
    ifstream infile;
    infile.open("big-data.csv");
    double dum1, dum2, dum3;
    char c;
    for (unsigned i = 0; i < size; i++) {
	infile >> x[i] >> c >> y[i] >> c >> dum1 >>c >> dum2 >> c >> dum3;
	cerr << "Read: " << x[i] << ", " << y[i] << endl;
    }


    for (unsigned i = 0; i < size; i++) {
	wi[i] = 1.0;
    }

    // Looks like both tol and wi are optional arguments, and only 
    cerr << "Calling spline\n";
    //spline_mp_spaps_(&x[0], y, &tol, wi, &spline, &size);
    c_spaps(x,y,tol,wi,2,&spline,size,1,0);
    cout<<"In C spline order"<<spline.order<<endl;
    cout<<"In C spline szk"<<spline.sizecoefs<<endl;
    cout<<"In C spline szcoefs"<<spline.sizeknots<<endl;
    for (unsigned i = 0; i < spline.sizeknots; i++) {
	    cout<<"In C spline knots "<<spline.knots[i]<<endl;
    }

    for (unsigned i = 0; i < spline.sizecoefs; i++) {
	    cout<<"In C spline coefs "<<spline.coefs[i]<<endl;
    }
    //cin.get();
    cerr << "Spline created. Compute spline values\n";
    for ( double i = 0; i < x[size-1]; i+= 0.1) {
	    //c_spaps(x,y,tol,wi,&spline,size);
	    cout << i << ": " << c_bsp_eval(&spline, &i) << endl;
	    //free(spline.knots);
	    //free(spline.coefs);

    }
    free(x);
    free(y);
    free(wi);

    free (spline.knots);
    free (spline.coefs);

}


main (int, char **) {
    //    testSmall();
    testBig();
}
