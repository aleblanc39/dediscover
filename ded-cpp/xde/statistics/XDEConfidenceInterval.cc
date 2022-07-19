#include "XDEConfidenceInterval.h"

using namespace std;

XDEConfidenceInterval::XDEConfidenceInterval() {

    // TODO_ALGM
   // addToControlParameters(std::make_shared<DoubleParameter>(
    //     "alpha", 0.05, 0.0, 1.0, true, true, "Significance level."));

 
}





XDEConfidenceInterval::~XDEConfidenceInterval() {}

/**
 * Validates if the parameters listed in the estimator result are all
 * defined  and set to estimation in the estimator set, and that all
 * the estimated parameters in the parameter set are defined in the
 * estimator result.
 *
 * This method will say nothing about all the non-estimated parameters
 * used to create the estimator result are part of parameter set.
 */

bool XDEConfidenceInterval::validateParamSets(
    EstimatorResultPtr estimatorResult,
    EstimationParameterSetPtr parameterSet) {
    vector<string> paramNames = estimatorResult->getEstimatedParameterNames();

    if (paramNames.size() != parameterSet->nbEstimatedParameters())
        return false;

    for (const auto &s : paramNames) {
        if (!parameterSet->isEstimated(s)) return false;
    }
    return true;
}
