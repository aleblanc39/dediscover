#include <base/XDEBlas.h>

#include <vector>

class MathUtil {
   public:
    /**
     * Interpolate between two points. Does not extrapolate or throw errors.
     * Values on boundary are used when out of bound.
     * */

    static void straightLineInterpolation(const std::vector<double> &timepts,
                                          const double &x,
                                          const TDoubleMatrix &y,
                                          TDoubleVector &ret);


    static bool invertMatrix(const TDoubleMatrix &input,
                      TDoubleMatrix &inverse);
};