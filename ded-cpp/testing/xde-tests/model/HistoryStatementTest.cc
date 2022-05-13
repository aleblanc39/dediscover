/**
 * Program to test a model containing delayed variables and an history function.
 */

#include <TestUtils.h>
#include <base/XDEMessage.h>
#include <expressions/ExpressionHandler.h>
#include <model/XDEModel.h>
#include <stdarg.h>

#include <boost/lexical_cast.hpp>
#include <iostream>

#include "doctest.h"

using namespace std;

// void compute(int testID, DDEModel &model, double t,
//              TDoubleVector &initConditionValues, TDoubleVector &depVarValues,
//              TDoubleVector &modelParamValues, TDoubleMatrix &delayValues,
//              vector<double> &expectedResults);

SCENARIO("Tests of  history statements") {
    GIVEN("A model with delay statements") {
        XDEModel dde(
            "dX/dt = 3*X[t0] + alpha * Y_\n\
            dY_/dt = 2*Y_[t0] + X[t-1] + beta\n\
            dZ/dt = 3*Y_[t0]  * beta + X[t-5]\n\
            Y_[t] = 1/t, -INF < t < -5\n\
            Y_[t] = 1/t^2, -5 <= t < 0\n\
            X[t] = 2*t + alpha, -5 <= t < 0\n");

        MemorySynchronizerPtr mem = dde.getMemorySynchronizer();
        mem->updateMemory("alpha", 11);
        mem->updateMemory("beta", 50);

        mem->updateMemory(ExpressionHandler::getInitialConditionString("X"), 1);
        mem->updateMemory(ExpressionHandler::getInitialConditionString("Y_"),
                          2);

        auto valuesAtM10 = dde.getHistoryAtTime(-10);
        auto valuesAtM5 = dde.getHistoryAtTime(-5);
        auto valuesAtM2 = dde.getHistoryAtTime(-2);

        map<string, int> varLocs;
        int i = 0;
        for (const auto depVar: dde.getDependentVariables()) {
            varLocs[depVar -> getName()] = i++;
        }


        THEN("Value of Y_ at time -10 is 0.1") {
            CHECK_EQ(valuesAtM10[varLocs["Y_"]], -0.1);
        }
        // THEN("Value of Y_ at time -5 is 0.04") {
        //     CHECK(dde.getHistory("Y_", -5) == 0.04);
        // }
        // THEN("Value of Y_ at time -2 is 0.25") {
        //     CHECK(dde.getHistory("Y_", -2) == 0.25);
        // }
        // THEN("Value of X at time -2 is 7") {
        //     CHECK(dde.getHistory("X", -2) == 7.0);
        // }
        // THEN("Value of X at time -10 (out of defined range) is 0") {
        //     CHECK(dde.getHistory("X", -10) == 0.0);
        // }
    }
}

SCENARIO("Tests of computation with history statements") {
    GIVEN("A model with delay statements") {
        XDEModel dde(
            "dX/dt = 3*X[t0] + alpha * Y_\n\
            dY_/dt = 2*Y_[t0] + X[t-1] + beta\n\
            dZ/dt = 3*Y_[t0]  * beta + X[t-5]\n\
            Y_[t] = 1/t, -INF < t < -5\n\
            Y_[t] = 1/t^2, -5 <= t < 0\n\
            X[t] = 2*t + alpha, -5 <= t < 0\n");

        vector<string> depVarNames = getAllNames(dde.getDependentVariables());
        vector<string> modelParamNames = getAllNames(dde.getModelParameters());

        TDoubleVector depVarValues(depVarNames.size());
        TDoubleVector modelParamValues(modelParamNames.size());

        TDoubleVector initialConditionValues(depVarNames.size());

        vector<double> expectedResults(depVarNames.size());

        TDoubleMatrix delayValues(depVarNames.size(),
                                  dde.getUniqueTimes(0).size(), 2.4);

        depVarValues[findElementIndex(depVarNames, string("X"))] = 10;
        depVarValues[findElementIndex(depVarNames, string("Y_"))] = 20;
        depVarValues[findElementIndex(depVarNames, string("Z"))] = 30;

        modelParamValues[findElementIndex(modelParamNames, string("alpha"))] = -2;
        modelParamValues[findElementIndex(modelParamNames, string("beta"))] = -32;

        initialConditionValues[findElementIndex(depVarNames, string("X"))] = 0;
        initialConditionValues[findElementIndex(depVarNames, string("Y_"))] = 1;
        initialConditionValues[findElementIndex(depVarNames, string("Z"))] = 2;

        dde.setDelayedVariableValues(delayValues);

        TDoubleVector yout(depVarValues.size());
        dde.setTimeVariableValue(0);
        dde.setFixedInitialConditionValues(initialConditionValues);

        // Why delayValues set in two differnt places?
        dde.compute(0, depVarValues, modelParamValues, delayValues,  yout); 

        THEN("Computed value of X should be -40"){CHECK(yout[findElementIndex(depVarNames, string("X"))] == -40.0);}
        THEN("Computed value of Y_ should be -27.6"){CHECK(yout[findElementIndex(depVarNames, string("Y_"))] == -27.6);}
        THEN("Computed value of Z should be -93.6"){CHECK(yout[findElementIndex(depVarNames, string("Z"))] == -93.6);}
    }

}
