#ifndef ONESTEPESTIMATOR_H_
#define ONESTEPESTIMATOR_H_

#include <estimator/XDEEstimator.h>

class SolverBasedEstimator;

typedef std::shared_ptr<SolverBasedEstimator> SolverBasedEstimatorPtr;

/**
 * \brief Estimation using the solver-based method.
 *
 * Class to perform the solver-based estimation. The estimator class
 * by itself doesn't do much. Most of the work is performed by the
 * Optimizer object.
 */

class SolverBasedEstimator : public XDEEstimator {
   public:
    SolverBasedEstimator() {
        std::cerr << "Creating empty SolverBasedEstimator" << std::endl;
    }
    // SolverBasedEstimator(const SolverBasedEstimator &s) : XDEEstimator(s) {}
    SolverBasedEstimator(ModelPtr model, SolverPtr solver,
                         ObjectiveFunctionPtr objFcn, OptimizerPtr optimizer,
                         DataTableMappingPtr m);
    virtual ~SolverBasedEstimator(){};

    static std::string s_getMethodName() { return "Object Function Estimator"; }

    static std::string s_getShortMethodDescription() {
        return "Object Function Estimator for Least Square and Max Likelihood";
    }

    static std::string s_getLongMethodDescription() {
        return "A longer description.";
    }

    static std::vector<std::string> s_getMethodAttributes() {
        return std::vector<std::string>();
    }

    EstimatorResultPtr estimate(EstimationParameterSetPtr parameterSet, const ParameterValueMap &pvm);

    double estimateResultingObjectSize();

   protected:
   private:
    double computeRSS(OptimizationResultsPtr optimizationResult,
                      EstimationParameterSetPtr estimationParamSet);
};

#endif /*ONESTEPESTIMATOR_H_*/
