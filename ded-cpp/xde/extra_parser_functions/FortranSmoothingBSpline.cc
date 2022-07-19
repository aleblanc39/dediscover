#include "FortranSmoothingBSpline.h"

#include <algorithms/ConcreteParameters.h>

#include <boost/cast.hpp>
#include <fstream>
#include <boost/make_shared.hpp>
using namespace std;

// Declare the external fortran spline methods.

const size_t FSmoothingBSpline::max_size = 1000;

extern "C" {
void c_spaps(double *x, double *y, double tol, double *wi, int order,
             FSmoothingBSpline::sp *spline, int size, int autoweight,
             int usegcv);

double c_bsp_eval(FSmoothingBSpline::sp *spline, double *x);

double c_bsp_evald(FSmoothingBSpline::sp *spline, double *x);
}

const std::string FSmoothingBSpline::gcvOption =
    "gcv";
const std::string FSmoothingBSpline::toleranceOption =
    "tolerance";
const std::string FSmoothingBSpline::orderOption = "Order";
FSmoothingBSpline::FSmoothingBSpline() {

    // TODO_ALGM
    
    // addToControlParameters(std::make_shared<BoolParameter>(
    //     gcvOption, true, false, true, false, false,
    //     "Automatically determine tolerance using GCV"));

    //  addToControlParameters(std::make_shared<DoubleParameter>(
    //     toleranceOption, -0.5, 2, 0, false, false,
    //     "Tolerance level"));

    //   addToControlParameters(std::make_shared<IntParameter>(
    //     orderOption, 2, 1, 5, true, true,
    //     "Order"));

  

    //     controlParameters.push_back(GeneralParameterPtr(
    //         new BoolParameter(getUseGCVOption(), true, false, true, false,
    //         false,
    //                           "Tolerance level")));

    //     controlParameters.push_back(GeneralParameterPtr(new DoubleParameter(
    //         getToleranceOption(), -0.5, 2, 0, false, false, "Tolerance
    //         level")));

    //     controlParameters.push_back(GeneralParameterPtr(
    //         new IntParameter(getOrderOption(), 2, 1, 5, true, true,
    //         "Order")));
}

FSmoothingBSpline::~FSmoothingBSpline() {}

double FSmoothingBSpline::computeDerivative(double t) {
    double d = c_bsp_evald(&spline, &t);
    return d;
}

double FSmoothingBSpline::computeFunction(const double &t) {
    double tp = t;
    double d = c_bsp_eval(&spline, &tp);
    // std::cerr << "FSpline computed at: " << t << ", returned : " << d <<
    // std::endl;
    return d;
}

void FSmoothingBSpline::initializeSpecific() {
    TDoubleVector times;
    TDoubleVector values;
    extractSortedPoints(times, values, AVERAGE_REPEATS);

    size_t size = times.size();
    if (size > max_size)
        throw XDEException("Maximum size for the FSmoothingBspline is " +
                           boost::lexical_cast<string>(max_size) +
                           " timepoints");
    if (size <= 3) {
        throw XDEException(
            "Error building Smoothing Spline: need at least 4 different "
            "timepoints.");
    }
    
    double tol = getDoubleParameterValue(toleranceOption);
    int order = getIntParameterValue(orderOption);
    bool usegcv = getBoolParameterValue(gcvOption);

    if ((((int)size) - ((int)order)) < 2) {
        std::cerr << "Will throw exception\n";
        throw XDEException("Order too high for the number of data points.");
    }
    double *x = (double *)malloc(size * sizeof(double));
    double *y = (double *)malloc(size * sizeof(double));
    double *wi = (double *)malloc(size * sizeof(double));
    for (size_t i = 0; i < size; i++) {
        x[i] = times[i];
        y[i] = values[i];
        wi[i] = 1.0;
    }
    std::cerr << "Calling c_spaps\n";
    std::cerr << "Arrays of size " << size << std::endl;

    c_spaps(x, y, tol, wi, order, &spline, size, 1, usegcv ? 1 : 0);
    std::cerr << "Returned from c_spaps\n";

    free(x);
    free(y);
    free(wi);
}
