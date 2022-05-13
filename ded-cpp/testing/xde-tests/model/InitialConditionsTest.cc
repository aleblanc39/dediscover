
#include <TestUtils.h>
#include <model/XDEModel.h>

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <doctest.h>
using namespace std;

SCENARIO ("Looking at initial conditions settings") {

    GIVEN("A model with two initial conditions assigned to numbers") {
        XDEModel model("dx/dt = x+1\n\
                dy/dt = 3*alpha\n\
                x[t0] = 1\n\
                y[t0]=2\n");
        TDoubleVector initConds(2);
        model.initializeInitialConditionValues(initConds);
        THEN("Initial value of x is 1"){CHECK(initConds[findElementIndex(model.getDependentVariables(), "x")]==1);}
        THEN("Initial value of y is 2"){CHECK(initConds[findElementIndex(model.getDependentVariables(), "y")]==2);}
    }


    GIVEN("A model with two initial conditions that contain formulas and dependencies") {
        XDEModel model("dx/dt = x+1\n\
                dy/dt = 3*beta\n\
                x[t0] = 5+alpha\n\
                MAC1 = x[t0] + 20\n\
                y[t0]=2*MAC1\n");

        TDoubleVector initConds(2);
        TDoubleVector modelParams(2);
        modelParams(findElementIndex(model.getModelParameters(), "beta")) = 4;
        modelParams(findElementIndex(model.getModelParameters(), "alpha")) = 11.3;

        model.setModelParameterValues(modelParams);
        model.initializeInitialConditionValues(initConds);

        THEN("Initial value of x is 16.3"){CHECK(initConds[findElementIndex(model.getDependentVariables(), "x")]==16.3);}
        THEN("Initial value of y is 72.6"){CHECK(initConds[findElementIndex(model.getDependentVariables(), "y")]==72.6);}
    }
}
