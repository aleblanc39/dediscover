#ifndef XDEESTIMATOR_H_
#define XDEESTIMATOR_H_

#include <algorithms/XDEAlgorithm.h>
#include <objectfunction/DataGenerator.h>
#include <objectfunction/XDEObjectiveFunction.h>
#include <optimizer/OptimizerFactory.h>
#include <optimizer/XDEOptimizer.h>
#include <parameters/EstimationSymbolAttributes.h>
#include <solver/XDESolverWrapper.h>
#include <statistics/EstimatorResult.h>
#include <mapping/DataTableMapping.h>
#include <string>

class XDEEstimator;

typedef std::shared_ptr<XDEEstimator> EstimatorPtr;

/**
 * \brief Base class for the estimator classes.
 *
 */

class XDEEstimator : public XDEAlgorithm {
   public:

    XDEEstimator(){}
    XDEEstimator(ModelPtr model, 
                SolverPtr s,
                 ObjectiveFunctionPtr o,
                 OptimizerPtr opt,
                 DataTableMappingPtr m)
        : mapping(m), optimizer(opt), objectiveFunction(o), model(model), solver(s) {}

    virtual ~XDEEstimator();

    virtual EstimatorResultPtr estimate(
        EstimationParameterSetPtr parameterSet, const ParameterValueMap &) = 0;

    /** Utility method to create a parameter set based on the model. */

    OptimizerPtr getOptimizer();
    OptimizerPtr getOptimizer() const { return optimizer; }

    ObjectiveFunctionPtr getObjectiveFunction() const {
        return objectiveFunction;
    }

    SolverPtr getSolver() const { return solver; }

    /** Copy constructor. */
    XDEEstimator(const XDEEstimator &e);

    /** If the derived class does not implement then return 0. */
    virtual double estimateResultingObjectSize() { return 0; }

   protected:
    std::string ciName;
    static OptimizerFactory optimizerFactory;
    DataTableMappingPtr mapping;
    OptimizerPtr optimizer;
    ObjectiveFunctionPtr objectiveFunction;
    ModelPtr model;
    SolverPtr solver;

    void logParameterInfo(EstimationParameterSetPtr ps,
                          XDEMessage::MsgLevel level = XDEMessage::INFO);

   private:
};

#endif /*XDEESTIMATOR_H_*/
