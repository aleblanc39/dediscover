#include <control_point.h>
#include <extra_parser_functions/BoostSpline.h>
#include <knot_vector.h>
#include <exception>

void BoostSpline::initializeSpecific() {
    TDoubleVector x_sorted;
    TDoubleVector y_sorted;
    extractSortedPoints(x_sorted, y_sorted, AVERAGE_REPEATS);

    std::vector<double> y(y_sorted.size());
    std::copy(y_sorted.begin(), y_sorted.end(), y.begin());


    if (x_sorted.size() < 4)
        throw XDEException(
            "Problem construction Boost Spline: requires at least 4 distinct "
            "points.");

    /*
    * The boost spline requires values for the left and right endpoint derivatives when there are only 4 points. Will set them to 0 for now.
    * Could make them parameters later.
    * */

    double leftDerivative = (y.size() == 4)?0:std::numeric_limits<double>::quiet_NaN();
    double rightDerivative = (y.size() == 4)?0:std::numeric_limits<double>::quiet_NaN();

    std::cerr << "Creating boost spline\n";
    try {
    boost_spline = std::make_shared<boost::math::cubic_b_spline<double>>(y.data(), x_sorted.size(), x_sorted[0], x_sorted[1] - x_sorted[0], leftDerivative, rightDerivative);
    } catch ( std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
    std::cerr << "Done\n";

}

double BoostSpline::computeFunction(const double &t) {
    try {
    double d = (*boost_spline)(t);
    return d;
    } catch ( std::exception &ex) {
              return 0;
          }
}
