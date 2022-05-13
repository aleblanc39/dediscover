#ifndef XDEBLAS_H
#define XDEBLAS_H


#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <boost/numeric/ublas/matrix_proxy.hpp>
namespace UBLAS = boost::numeric::ublas;

typedef UBLAS::matrix<double>   TDoubleMatrix;
typedef UBLAS::vector<double>   TDoubleVector;


// template <class Tv>
// UBLAS::vector<Tv> getTVector(const std::vector<Tv>& vec);


template <class Tv>
UBLAS::vector<Tv> getTVector(const std::vector<Tv>& vec) {
    UBLAS::vector<Tv> ret(vec.size());
    for (unsigned i = 0; i < vec.size(); i++) ret[i] = vec[i];
    return ret;
}

template <class Tm, class Ts>
void setMatrixValue(UBLAS::matrix<Tm>& m, const Ts& scalar)
{
	bool moreRows = m.size1() > m.size2(); 
	int  maxRC = moreRows ? m.size1() : m.size2();
	int  minRC = moreRows ? m.size2() : m.size1();
	UBLAS::scalar_vector<Tm> vTemp(maxRC, scalar);

	for (int i=0; i<minRC; i++)
	{
		if (true == moreRows)
			column(m,i) = vTemp;
		else
			row(m,i) = vTemp;
	}
}



#endif