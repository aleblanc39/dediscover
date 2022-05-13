#include "objectfunction/LeastSquare.h"

#include <algorithms/ConcreteParameters.h>
#include <base/CBIMDefinitions.h>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

using namespace std;

LeastSquare::LeastSquare() {
    // controlParameters.clear();
    std::vector<std::string> v = getValidFunctions();

    addToControlParameters(std::make_shared<StringParameter>(
        getAppliedFunctionParameterName(), v[1], v, "", "", false, false,
        "Function applied to the difference of the values"));

    addToControlParameters(std::make_shared<BoolParameter>(
        getNormalizedParameterName(), false, false, false, false, false,
        "Indicates whether the data will be normalized."));

    addToControlParameters(std::make_shared<BoolParameter>(
        getLogValuesParameterName(), false, false, false, false, false,
        "Indicates whether the log of the values will be used."));
}

LeastSquare::~LeastSquare() {}

vector<string> LeastSquare::s_getMethodAttributes() {
    vector<string> attrs = {"solving"};
    return attrs;
}

/**
 * Preprocessing step. The computeSum method of this class will be
 * called multiple times, so this step is needed.
 *
 * Main function is to initialize the data generator based on the
 * parameters that will be used.
 *
 */
void LeastSquare::preProcess() {
    nbCalls = 0;
    XDE_ASSERT(dataGenerator != NULL);
    bool logData = getBoolParameterValue(getLogValuesParameterName());
    string lossFunction = getStringParameterValue(getAppliedFunctionParameterName());
    bool applyWeights = getBoolParameterValue(getNormalizedParameterName());
 
    DataGenerator::ScalingMode scalingMode = DataGenerator::NONE;
    if (applyWeights) {
        scalingMode = (lossFunction == getValidFunctions()[0])
                          ? DataGenerator::LINEAR
                          : DataGenerator::QUADRATIC;
    }
    dataGenerator->preProcess(logData, scalingMode);
}
