#include <doctest.h>
#include <parser/DelayedVariablesHandler.h>

using namespace std;

static double func(unsigned eq) { return eq * eq; }

SCENARIO("Testing the DelayedVariablesHandler class") {
    GIVEN("A DelayedVariablesHandler object") {
        vector<string> varNames = {"x", "y", "z"};
        vector<unsigned> ids = {3, 5, 88};

        vector<pair<string, unsigned>> delayedVars = {
            pair("x", 3), pair("x", 88), pair("y", 3), pair("x", 5)};

        DelayedVariableHandler<unsigned> dvh =
            DelayedVariableHandler<unsigned>(varNames, delayedVars, ids);

        std::function<double(unsigned)> thisFunc = &func;
        vector<double> uniqueTimes = dvh.getUniqueTimes(thisFunc);
        vector<double> uniqueTimesExpected = {9, 25, 88 * 88};

        TDoubleMatrix resultMat(varNames.size(), ids.size());
        resultMat(0, 0) = 10;
        resultMat(0, 2) = 11;
        resultMat(1, 0) = 9;
        resultMat(0, 1) = 8;

        std::vector<double> rearrangedVals =
            dvh.arrangeDelayVariableValues(resultMat);
        std::vector<double> expectedRearranged = {10, 11, 9, 8};
        THEN("There should be valid delay times") {
            CHECK_EQ(uniqueTimes, uniqueTimesExpected);
        }
        THEN("Rearranged solution is correct") {
            CHECK_EQ(rearrangedVals, expectedRearranged);
        }
    }
}
