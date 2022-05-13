#include <control_point.h>
#include <extra_parser_functions/MfcatsSpline.h>
#include <knot_vector.h>

const std::string MfcatsSpline::orderParam = "Order";

void MfcatsSpline::initializeSpecific() {
    std::cerr << "Initializing MfcatsSpline:\n";
    TDoubleVector x_sorted;
    TDoubleVector y_sorted;
    extractSortedPoints(x_sorted, y_sorted, AVERAGE_REPEATS);

    std::vector<double> y(y_sorted.size());
    std::copy(y_sorted.begin(), y_sorted.end(), y.begin());


    if (x_sorted.size() < 3)
        throw XDEException(
            "Problem construction B-spline: requires at least 3 distinct "
            "points.");

    // // Temporary, assume 4 CP
    // // XDE_ASSERT(y_sorted.size() == 4);
    // firstPoint = x_sorted[0];
    // lastPoint = x_sorted[x_sorted.size() - 1];

    // std::array<Degree, 1> degree = {Degree{3}};

    // std::vector<baf::ControlPoint> control_points;
    // // Hard coded for degree 3. Repeat second and third point
    // // for (unsigned i = 0; i < x_sorted.size(); i++) {
    // //     control_points.push_back(baf::ControlPoint({x_sorted[i],
    // //     y_sorted[i]}));
    // // }
    // control_points.push_back(baf::ControlPoint({x_sorted[0], y_sorted[0]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[1], y_sorted[1]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[1], y_sorted[1]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[1], y_sorted[1]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[2], y_sorted[2]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[2], y_sorted[2]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[2], y_sorted[2]}));
    // control_points.push_back(baf::ControlPoint({x_sorted[3], y_sorted[3]}));
    // // Create knots vector. For now assume 4 CP, at the same location as the
    // // control points.

    // double span = (x_sorted[3] - x_sorted[0]) / 5.0;
    // KnotVectors<1> knot_vector_ptr = {std::make_shared<baf::KnotVector>(
    //     baf::KnotVector({ParamCoord{x_sorted[0]}, ParamCoord{x_sorted[0]},
    //                      ParamCoord{x_sorted[0]}, ParamCoord{x_sorted[0]},

    //                      ParamCoord{x_sorted[0] + span},
    //                      ParamCoord{x_sorted[1] + 2*span},
    //                      ParamCoord{x_sorted[2] + 3*span},
    //                      ParamCoord{x_sorted[0] + 4*span},

    //                      ParamCoord{x_sorted[3]}, ParamCoord{x_sorted[3]},
    //                      ParamCoord{x_sorted[3]}, ParamCoord{x_sorted[3]}}))};

    // b_spline_ptr = std::make_shared<spl::BSpline<1>>(knot_vector_ptr, degree,
    //                                                  control_points);

    
    boost_spline = std::make_shared<boost::math::cubic_b_spline<double>>(y.data(), x_sorted.size(), x_sorted[0], x_sorted[1] - x_sorted[0]);
}

double MfcatsSpline::computeFunction(const double &t) {
    
    //return b_spline_ptr->Evaluate({ParamCoord{t}}, {0, 1})[1];
    return (*boost_spline)(t);
}
