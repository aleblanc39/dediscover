#ifndef XDEOPTIMIZERWRAPPER_H_
#define XDEOPTIMIZERWRAPPER_H_

#include <algorithms/XDEAlgorithm.h>
#include <algorithms/ParameterValue.h>
#include <objectfunction/XDEObjectiveFunction.h>
#include <optimizer/OptimizationResults.h>



class XDEOptimizer;

typedef std::shared_ptr<XDEOptimizer> OptimizerPtr;

/**
 * \brief Pure virtual base class for all the optimizer classes.
 *
 * Optimization will be initialized from the optimize method in this class,
 * which will call the pure virtual performOptimization that the implementing
 * class must provide.
 */

class XDEOptimizer : public XDEAlgorithm {
   public:
    XDEOptimizer();
    virtual ~XDEOptimizer();

    ObjectiveFunctionPtr getObjectiveFunction() { return objectiveFunction; }

    // temp_results the place to store intermediate output, No
    // intermediate output will be produced if control
    // parameter--refresh is < 1

    // bestmem        the best parameter found
    // bestval        the best (minimum) value of objective function
    // nfeval         number of function evaluation
    OptimizationResultsPtr optimize(ObjectiveFunctionPtr objectiveFunction,
                          EstimationParameterSetPtr parameterSet,
                          const ParameterValueMap &pvm);

    virtual double estimateResultingObjectSize() { return 0; }

   protected:
    ObjectiveFunctionPtr objectiveFunction;

    virtual OptimizationResultsPtr performOptimization(const ParameterValueMap &pvm) = 0;

    /**
     * Static function used to compute the objective function. The
     * void pointer user_data is expected to be of the XDEOptimizer
     * type. It is to be used by certain estimators.
     */

    static double computeObjectiveFunction(const TDoubleVector &params,
                                           void *user_data);

    /**
     * Not all these values will be used by all the optimizers, but
     * they will be used by enough of them to justify setting the
     * values in a preprocessing step.
     */
    TDoubleVector minBounds;
    TDoubleVector maxBounds;
    TDoubleVector initialValues;

    // TODO: rename setBounds. Actually assigns initial values as well.
    void setBounds(
        const std::vector<EstimationSymbolAttributes> &estimatedParameters);

    // TODO Should pass objFcn instead
    void preProcess(EstimationParameterSetPtr parameterSet, const ParameterValueMap &objFcnParamValues);
};

#endif /*XDEOPTIMIZERWRAPPER_H_*/
