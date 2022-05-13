
#ifndef CONFIDENCEINTERVALRESULT_H_
#define CONFIDENCEINTERVALRESULT_H_

#include <base/AlgorithmResult.h>
#include <base/XDEBlas.h>


#include <string>
#include <vector>

#include <optimizer/OptimizationResults.h>

class ConfidenceIntervalResult;

typedef std::shared_ptr<ConfidenceIntervalResult> ConfidenceIntervalResultPtr;


/**
 * \brief Class to handle the results of computing the confidence interval.
 */

class ConfidenceIntervalResult  {
   public:
    ConfidenceIntervalResult(OptimizationResultsPtr optimizationResults,
    const std::vector<std::string> &parameterNames,
    const std::vector<double> &standardError,
    const std::vector<double> &lowerBounds,
    const std::vector<double> &upperBounds,
    const std::vector<double> &tstat,
    const std::vector<double> &pvalues) : _optimizationResults(optimizationResults), _parameterNames(parameterNames),
        _standardError(standardError), 
        _lowerBounds(lowerBounds),
        _upperBounds(upperBounds),
        _tstat(tstat),
        _pvalues(pvalues){}


    virtual ~ConfidenceIntervalResult(){}

    std::vector<std::string> getParameterNames() const {
        return _parameterNames;
    }


    std::vector<double> getParameterValues() const { return _optimizationResults->getParamValues(); }
    std::vector<double> getStandardError() const { return _standardError; }
    std::vector<double> getLowerBounds() const { return _lowerBounds; }
    std::vector<double> getUpperBounds() const { return _upperBounds; }
    std::vector<double> getTstat() const { return _tstat; }
    std::vector<double> getPvalues() const { return _pvalues; }

   private:

    OptimizationResultsPtr _optimizationResults;
    std::vector<std::string> _parameterNames;
    std::vector<double> _standardError;
    std::vector<double> _lowerBounds;
    std::vector<double> _upperBounds;
    std::vector<double> _tstat;
    std::vector<double> _pvalues;
};

#endif /* CONFIDENCEINTERVALRESULT_H_ */
