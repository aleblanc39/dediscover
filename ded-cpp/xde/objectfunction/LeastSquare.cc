#include "objectfunction/LeastSquare.h"
#include <base/CBIMDefinitions.h>
#include <boost/lexical_cast.hpp>

using namespace std;

/**
 * Preprocessing step. The computeSum method of this class will be
 * called multiple times, so this step is needed.
 *
 * Main function is to initialize the data generator based on the
 * parameters that will be used.
 *
 */
void LeastSquare::preProcess(const ParameterValueMap &pvm ) {
    nbCalls = 0;
    XDE_ASSERT(dataGenerator != NULL);
    bool logData = pvm.getBoolParameterValue(getLogValuesParameterName());
    string lossFunction = pvm.getStringParameterValue(getAppliedFunctionParameterName());
    bool applyWeights = pvm.getBoolParameterValue(getNormalizedParameterName(), false);
 
    DataGenerator::ScalingMode scalingMode = DataGenerator::NONE;
    if (applyWeights) {
        scalingMode = (lossFunction == getValidFunctions()[0])
                          ? DataGenerator::LINEAR
                          : DataGenerator::QUADRATIC;
    }
    dataGenerator->preProcess(logData, scalingMode);
}
