
#include "EstimatorResult.h"

#include <base/XDEException.h>
#include <base/XDEUtil.h>
#include <math.h>

using namespace std;

// TODO Need the mapping here so can compute RSS
EstimatorResult::EstimatorResult(OptimizationResultsPtr optimizationResults,
                                 const std::vector<std::string> &paramNames,
                                 double RSS)
    : optimizationResults(optimizationResults),
      estimatedParameterNames(paramNames),
      rss(RSS) {

    double n = (double)optimizationResults->getNbObservations();
    double K = (double)paramNames.size();

    aic = n * log(rss / n) + 2 * K;
    bic = n * log(rss / n) + K * log(n);
    aicc = aic + 2 * K * (K + 1) / (n - K - 1);
}

