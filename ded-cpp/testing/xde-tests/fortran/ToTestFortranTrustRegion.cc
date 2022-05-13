/**
 * Basic program to test calling the fortran subroutine from C++. The
 * subroutine will be located in a shared library.
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <math.h>

using namespace std;

typedef struct  {
    double *knots;
    double *coefs;
    int order,sizeknots,sizecoefs;
} sp;


extern "C" {void c_nls_stir(double *x,
			 double *lb,
			 double *ub,
			 void f(double *x,double *f),
			 int sizep,int sizec);



}


void myfun(double *x,double *f)
{
	
	int k;	
          for (k=1;k<11;k++){
                f[k-1]=(2.0+2.0*k-exp(k*x[0])-exp(k*x[1]));
		//std::cout<<f[k]<<' ';
	  }
	  //std::cout<<std::endl;
}


main (int, char **) 
{
	int size = 2;
	double *x,*lb,*ub;
	x=(double*)malloc(size*sizeof(double));
	lb=(double*)malloc(size*sizeof(double));
	ub=(double*)malloc(size*sizeof(double));

	x[0]=1.0;
	x[1]=1.0;

	for (int i=0;i<2;i++) lb[i]=-1.0/0.0;//!20d0
	for (int i=0;i<2;i++) ub[i]=1.0/0.0;

	c_nls_stir(x,lb,ub,myfun,size,10);
	std::cout<<"x c result "<<x[0]<<" "<<x[1]<<std::endl;

} 


