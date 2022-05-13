#include <statistics/MathUtil.h>

 #include <boost/numeric/ublas/vector.hpp>
 #include <boost/numeric/ublas/vector_proxy.hpp>
 #include <boost/numeric/ublas/matrix.hpp>
 #include <boost/numeric/ublas/triangular.hpp>
 #include <boost/numeric/ublas/lu.hpp>
 #include <boost/numeric/ublas/io.hpp>

namespace ublas = boost::numeric::ublas;

void MathUtil::straightLineInterpolation(const std::vector<double> &timepts,
                                         const double &x,
                                         const TDoubleMatrix &y,
                                         TDoubleVector &ret) {
    ret.resize(y.size1());

    auto indx =
        std::lower_bound(timepts.begin(), timepts.end(), x) - timepts.begin();

    if (indx == 0 || (timepts[indx] == timepts[indx - 1])) {
        ret = row(y, indx);
        return;
    }

    if (indx == timepts.size()) {
        ret = row(y, timepts.size() - 1);
        return;
    }

    ret = row(y, indx - 1) +
          (row(y, indx) - row(y, indx - 1)) *
              ((x - timepts[indx - 1]) / (timepts[indx] - timepts[indx - 1]));
}

bool MathUtil::invertMatrix(const TDoubleMatrix &input,
                            TDoubleMatrix &inverse) {
    //typedef permutation_matrix<std::size_t> pmatrix;
    using namespace boost::numeric::ublas; 
    typedef permutation_matrix<std::size_t> pmatrix;

    // create a working copy of the input
    TDoubleMatrix A(input);
    // create a permutation matrix for the LU-factorization
    pmatrix pm(A.size1());

    // perform LU-factorization
    int res = lu_factorize(A, pm);
    if (res != 0) return false;

    // create identity matrix of "inverse"
    inverse.assign(ublas::identity_matrix<double>(A.size1()));

    // backsubstitute to get the inverse. 
    try {
    lu_substitute(A, pm, inverse);
    } catch (...) {
        return false;
    }
    return true;
}
