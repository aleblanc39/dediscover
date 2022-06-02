#include <TestUtils.h>
#include <base/XDEException.h>
#include <extra_parser_functions/LOCF.h>
#include <model/XDEModel.h>
#include <iostream>
#include <cmath>
#include "doctest.h"

using namespace std;

SCENARIO("Have a model with a TVF, using dep var instead of time variable") {
    XDEMessage::setLogLevel(XDEMessage::DEBUG3);

    XDEModel model("dalpha/dt = 12 +  locf(alpha, [t1, 10, 20], [5, 9, beta])\n\
	    X = 3*alpha/10\n\
        alpha[t0] = 1\n");

    TDoubleVector y(1);
    TDoubleVector modelParams(2);
    modelParams[findElementIndex(model.getModelParameters(),"t1")] = 1;
    modelParams[findElementIndex(model.getModelParameters(),"beta")] = 3;
    TDoubleVector yout(y.size());

    GIVEN("The current value of alpha is 3") {
        
        y[0] = 3;  // Will be current value of alpha
        model.initializeTVFunctions();
        model.compute(0, y, modelParams, yout);
        THEN("Next value of alpha is 17"){CHECK(yout[0] == 17.0);}
    }

    GIVEN("The current value of alpha is 15") {
        
        y[0] = 15;  // Will be current value of alpha
        model.initializeTVFunctions();
        model.compute(0, y, modelParams, yout);
        THEN("Next value of alpha is 21"){CHECK(yout[0] == 21.0);}
    }
    GIVEN("The current value of alpha is 100") {
        
        y[0] = 100;  // Will be current value of alpha
        model.initializeTVFunctions();
        model.compute(0, y, modelParams, yout);
        THEN("Next value of alpha is nan"){CHECK(std::isnan(yout[0]));}
    }
}
