#include <doctest.h>
#include <extra_parser_functions/BoostSpline.h>
#include <model/XDEModel.h>

#include <cmath>

SCENARIO("Test for BoostSpline") {
    GIVEN("A new spline object") {
        BoostSpline spline;
        spline.addControlPoint(TimeVaryingEntry(0.0), TimeVaryingEntry(11.0));
        spline.addControlPoint(4.667, 4.667);
        spline.addControlPoint(9.333, 19.333);
        spline.addControlPoint(14, 4.0);

        spline.initialize();
        THEN("Value at 0 should be 11") {
            CHECK(std::abs(11.0 - spline.compute(0.0)) < 0.001);
        }
    }

    GIVEN("A new spline object that is initialized with a NaN") {
        BoostSpline spline;
        spline.addControlPoint(TimeVaryingEntry(0.0), TimeVaryingEntry(11.0));
        spline.addControlPoint(4.667, 4.667);
        spline.addControlPoint(9.333, std::numeric_limits<double>::quiet_NaN());
        spline.addControlPoint(14, 4.0);
        spline.addControlPoint(18.667, 1.0);
        spline.addControlPoint(23.333, std::numeric_limits<double>::quiet_NaN());


        spline.initialize();
        THEN("Value at 0 should be near 11") {
            CHECK(std::abs(11.0 - spline.compute(0.0)) < 0.001);
        }
    }

    GIVEN("A model that contains a boost spline") {
        XDEModel model(
            "dx/dt=1\neta = boost_spline(t, 3, [0, 4.667, 9.333, 14] , [11.0, "
            "4.667, 19.333, 4.0])");
        for (double i = 0; i < 14; i += 1) {
            // std::cerr << i << " -- " <<
        }
        THEN("Haven't crashed") { CHECK(true); }
    }
}