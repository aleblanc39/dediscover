#include "optimizer/XDEOptimizer.h"

//TODO Very specific here. Should not be needed
#include <parser/ModelParameter.h>
#include <parser/DependentVariable.h>

#include <boost/cast.hpp>

#include <parameters/EstimationSymbolAttributes.h>
#include <algorithms/ParameterValue.h>

using namespace std;


double XDEOptimizer::computeObjectiveFunction(const TDoubleVector &params, void *user_data) {
    XDEOptimizer *obj = (XDEOptimizer *) user_data;
    return obj -> getObjectiveFunction() -> computeFunction(params);
}

XDEOptimizer::XDEOptimizer() {
}

XDEOptimizer::~XDEOptimizer() {
}
void XDEOptimizer::preProcess(EstimationParameterSetPtr parameterSet, const ParameterValueMap &objFncParamValues) {
    XDE_ASSERT(objectiveFunction != NULL);

    // Because the optimizer will call the objective function multiple
    // times with a lot of parameters being the same, it will
    // explicitly initialize the ObjectiveFunction object.

    // First get the estimated parameters, so we know in which order they are expected.
    auto estimatedParameters = parameterSet->getParameters(&EstimationSymbolAttributes::isEstimated);


    // By default the model will not initialize the TimeVaryingFunction that do not contain variable time/locations
    // each time is initialized by the solver, since it will be useless most of the time.

    //model -> initializeConstantTVFunctions();
    objectiveFunction -> preProcess(objFncParamValues);
    setBounds(estimatedParameters);
}

/**
 * Set the minbounds and maxbounds arrays to be used by the
 * optimization function. Each parameter can be either an initial
 * condition or a model parameter. In the case of a model parameter if
 * it is a spline then the min/max are taken from each control point.
 * TODO: IMPORTANT: change the name. the method also sets the initial values.
 */
void XDEOptimizer::setBounds(const vector<EstimationSymbolAttributes> &estimatedParameters) {


    auto nbParams = estimatedParameters.size();
    XDE_ASSERT(nbParams > 0);

    minBounds.resize(nbParams);
    maxBounds.resize(nbParams);
    initialValues.resize(nbParams);

    transform(estimatedParameters.begin(), estimatedParameters.end(), minBounds.begin(), mem_fun_ref(&EstimationSymbolAttributes::getMinValue));
    transform(estimatedParameters.begin(), estimatedParameters.end(), maxBounds.begin(), mem_fun_ref(&EstimationSymbolAttributes::getMaxValue));
    transform(estimatedParameters.begin(), estimatedParameters.end(), initialValues.begin(), mem_fun_ref(&EstimationSymbolAttributes::getInitialValue));
}


/**
 * This optimize method() will be called for all the derived classes,
 * and so should not be redefined.
 */

OptimizationResultsPtr XDEOptimizer::optimize(ObjectiveFunctionPtr objfcn,
			    EstimationParameterSetPtr parameterSet,
                const ParameterValueMap &pvm) {

    /**
     * preProcess will set up the bounds and initial values based on
     * the parameter set, and initialize the objectiveFunction to set
     * up the mapping between how the parameter/initial conditions are
     * used by Solver.
     * 
     * TODO If nfeval is not used remove it
     *
     */

    // TODO Use std::function here, 

    objectiveFunction = objfcn;

    // TODO Send actual pvm for the obj fcn (first), then actually crete the obj fcn here.
    preProcess(parameterSet, {});
    return performOptimization(pvm);
    //ndata = boost::numeric_cast<unsigned>(objectiveFunction -> computeNumObservations());
}
