#include <boost/math/interpolators/cubic_b_spline.hpp>
#include <iostream>
int main() {
    // We begin with an array of samples:
    std::vector<double>  x = {0.0, 4.667, 9.333, 14.0};
    std::vector<double>  y = {11.0, 4.667, 19.333, 4.0};

    // We could define an arbitrary start time, but for now we'll just use 0:
    boost::math::cubic_b_spline<double> spline(y.data(), y.size(), 0, /* start time */ 4.667,-2,-1);
    std::cerr << "Spline built\n";
    for (double d = 0; d < 14; d+=0.1) {
        std::cerr << d  << "\t" <<  spline(d) << std::endl;
    }
}