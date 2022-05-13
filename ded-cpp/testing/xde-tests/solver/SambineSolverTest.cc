#include <solver/SambineDDEWrapper.h>
#include <solver/SandineDDEStruct.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <boost/make_shared.hpp>

#include "doctest.h"

using namespace std;

const static string eqns =
    "dx/dt = 2*x +x[t-1]\n\
        dy/dt = 3*y + x[t-alpha]\n\
        dz/dt = 3*y[t-2] + x[t-2] + 4\n\
        x[t] = -3, -100<t<=-50\n\
        x[t] = 42, -50<t<0\n\
        y[t] = 22, -100<t<0";

// FnHistory his(model, initialConditionValues, tspan);
// FnEvent ef;
// FnDelay fnd(model);

/**
 * Really need to test the functor that are provided to the solver.
 * */
SCENARIO("Test the DDes struct") {
    GIVEN("A model with some delays") {
        ModelPtr model = std::make_shared<XDEModel>(eqns);
        FnDdes ddes(model);
        THEN("Check that we do have 3 variables") {
            CHECK_EQ(3, model->getDependentVariables().size());
        }
        THEN("Compute  one iteration") {
            std::vector<double> y = {1000, 2000, 3000};
            std::vector<double> z = {1, 2, 3, 14, 15, 16, 27, 28, 29};

            auto result = ddes(2, y, z);
            for (auto d : result) {
                std::cerr << d << endl;
            }
            THEN("Right return size") { CHECK_EQ(result.size(), 3); }
        }
    }
}

SCENARIO("Check the attributes for DDE") {
    THEN("Attribute ODE is not set") {
        auto attrs = SambineDDEWrapper::s_getMethodAttributes();
        CHECK(find(attrs.begin(), attrs.end(), "ODE") == attrs.end());
    }

    THEN("Attribute DDE is set") {
        auto attrs = SambineDDEWrapper::s_getMethodAttributes();
        CHECK(find(attrs.begin(), attrs.end(), "DDE") != attrs.end());
    }
}