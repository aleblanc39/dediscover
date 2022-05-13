#include "DataGenerator.h"

#include <algorithm>
#include <mapping/DataTableMapping.h>
#include "ParameterMapping.h"

using namespace std;

DataGenerator::DataGenerator(ModelPtr model,
                            EstimationParameterSetPtr e,
                            DataTableMappingPtr m): model(model), mapping(m) {
    preprocessed = false;

    ParameterMapping::initializeMapping(modelParameterValues,    // to be filled
                                        initialConditionValues,  // to be filled
                                        parameterMapping,        // to be filled
                                        e,                       // const
                                        model);
}

DataGenerator::~DataGenerator() {}

/**
 * Initialize the transformation to be applied to the data. The
 * 'scaling' parameter determines if it is left unchanged or divided
 * by the variance (QUADRATIC) or the standard deviation (LINEAR) of
 * the values in the same column. Log is applied after the scaling.
 *
 */

void DataGenerator::initialize(bool applyLog) {
    transformData = &DataGenerator::no_op;
    if (applyLog) transformData = &DataGenerator::__log;
}

void DataGenerator::preProcess(bool applyLog, ScalingMode scaling) {
    DataGenerator::initialize(applyLog);
    // Put the raw observed values in the returnedPairs vector.
    preprocessSpecific();
    // Apply the scaling if required.
    if (scaling != NONE) {
        computeMultiplier(scaling);
        for (unsigned i = 0; i < returnedPairs.observed.size(); i++)
            returnedPairs.observed[i] *= multiplier[i];
    }

    if (applyLog) {
        transform(returnedPairs.observed.begin(), returnedPairs.observed.end(),
                  returnedPairs.observed.begin(),
                  bind1st(mem_fun(DataGenerator::transformData), this));
    }
}

void DataGenerator::preProcess(const std::vector<GeneralParameterPtr> &params) {
    // For now assume no scaling, log allowed.
    DataGenerator::initialize(false);
    preprocessSpecific(params);
}
