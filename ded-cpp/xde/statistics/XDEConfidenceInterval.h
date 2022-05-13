#ifndef XDECONFIDENCEINTERVAL_H_
#define XDECONFIDENCEINTERVAL_H_

#include <algorithms/XDEAlgorithm.h>
#include <estimator/XDEEstimator.h>
#include <objectfunction/DataGenerator.h>
#include <solver/XDESolverWrapper.h>
#include <statistics/ConfidenceIntervalResult.h>
#include <optimizer/OptimizationResults.h>

#include <boost/smart_ptr.hpp>

class XDEConfidenceInterval;

typedef std::shared_ptr<XDEConfidenceInterval> ConfidenceIntervalPtr;

/**
 * \brief Base class for Confidence Interval.
 *
 */

class XDEConfidenceInterval : public XDEAlgorithm {
   public:
    XDEConfidenceInterval();
    virtual ~XDEConfidenceInterval();

    virtual double estimateResultingObjectSize() { return 0; }

    virtual ConfidenceIntervalResultPtr evaluate(OptimizationResultsPtr optimizerResult,
                                               DataTableMappingPtr datatableMapping,
                                               EstimationParameterSetPtr parameterSet,
                                               ModelPtr model,
                                               SolverPtr solver) = 0;
    DataGeneratorPtr getDataGenerator() { return dataGenerator; }
    std::vector<double *> parameterMapping;

   protected:
    /**
     * Check that the estimated parameters from the parameter set
     * match the estimated parameters from the estimator results.
     */
    static bool validateParamSets(EstimatorResultPtr estimatorResult,
                                  EstimationParameterSetPtr parameterSet);
    DataGeneratorPtr dataGenerator;

    TDoubleVector modelParameterValues;
    TDoubleVector initialConditionValues;

    std::string getAlphaParameterName() { return "alpha"; }

   private:
};

#endif /* XDECONFIDENCEINTERVAL_H_ */
