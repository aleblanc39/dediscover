
#ifndef ESTIMATORRESULT_H_
#define ESTIMATORRESULT_H_

#include <base/AlgorithmResult.h>
#include <base/XDEBlas.h>
#include <optimizer/OptimizationResults.h>
#include <statistics/ConfidenceIntervalResult.h>

#include <boost/smart_ptr.hpp>

class EstimatorResult;

typedef std::shared_ptr<EstimatorResult> EstimatorResultPtr;

/**
 * \brief Handles the results from the estimation process.
 *
 * Includes methods to save tt and read from a file.
 */

class EstimatorResult : public AlgorithmResult {
   public:
    EstimatorResult(OptimizationResultsPtr optimizerResult,
                    const std::vector<std::string> &paramNames,
                    double RSS);

    virtual ~EstimatorResult() {}

    auto getOptimizationResults() { return optimizationResults; }
    int getNumberOfEvaluations() { return numberOfEvaluations; }
    double getOptimalValue() { return optimizationResults -> getOptimalValue(); }
    double getRSS() { return rss; }
    double getAIC() { return aic; }
    double getBIC() { return bic; }
    double getAICc() { return aicc; }
    int getNParam() { return nparam; }
    int getNObs() { return optimizationResults -> getNbObservations(); }

    const auto &getBestResult() { return optimizationResults->getParamValues(); }
    const auto &getEstimatedParameterNames() {
        return estimatedParameterNames;
    }

    double aic;
    double bic;
    double aicc;

   private:
    OptimizationResultsPtr optimizationResults;
    std::vector<std::string> estimatedParameterNames;
    double rss;

    int numberOfEvaluations;
    int nparam;
};

#endif /* ESTIMATORRESULT_H_ */
