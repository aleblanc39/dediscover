#include "solver/XDESolverWrapper.h"
#include <base/XDEUtil.h>
#include <algorithms/ConcreteParameters.h>

#include <algorithm>
using namespace std;

XDESolverWrapper::XDESolverWrapper() {
    useDefaultTspan = true;
    startTime = 0;
    stopTime = 5;
    stepSize = 0.1;
}

XDESolverWrapper::~XDESolverWrapper() {}

void XDESolverWrapper::setTSpan() {
    if (startTime >= stopTime)
        throw XDEException(
            "Error with the control parameters: stop time must be greater than "
            "start time.");

    if (stepSize <= 0)
        throw XDEException(
            "Error with the control parameters: step size must be greater than "
            "0.");

    int nstep = (int)((stopTime - startTime) / stepSize) + 1;
    tspan.resize(nstep);
    for (int i = 0; i < nstep; i++) tspan[i] = startTime + i * stepSize;
}


void XDESolverWrapper::setTSpan(const TDoubleVector &newTSpan) {
    tspan = newTSpan;
    startTime = tspan[0];
    stopTime = tspan[tspan.size()-1];
    useDefaultTspan = false;
}


// AL2020 Check if either of the following 2 methods is ever called.
// Also, should SimulationParameterSet be a const? SolverResult returned instead of
// passed as parameter?
// This method could be called undred of thousands of times during estimation. Creating a new
// result object on every call could be problematic.
void XDESolverWrapper::solve(ModelPtr model,
                             SimulationParameterSet &parameterSet,
                             SolverResult &solverResult,
                             const ParameterValueMap &pvm) {
    TDoubleVector tout;
    TDoubleMatrix yout;
    solve(model, parameterSet, tout, yout, pvm);
    vector<string> varNames;
    const vector<DependentVariablePtr> & depVars = model->getDependentVariables();
    std::transform(depVars.begin(), depVars.end(), varNames.begin(), [](DependentVariablePtr p) -> string {return p-> getName();});

    solverResult.setVariableNames(varNames);
    solverResult.setTimepoints(tout);
    solverResult.setData(yout);
}

void XDESolverWrapper::solve(ModelPtr model,
                             SimulationParameterSet &parameterSet,
                             TDoubleVector &tout, TDoubleMatrix &yout, const ParameterValueMap &pvm) {
    // First get the names of the model parameters in the correct
    // order.
    vector<string> modelParamNames =  getAllNames(model->getModelParameters());
    TDoubleVector modelParamValues(modelParamNames.size());
    unsigned i = 0;
    for (const string s : modelParamNames) {
        modelParamValues[i++] = parameterSet.getParameter(s).getValue();
    }

    // Same with initial conditions
    vector<string> initialConditionNames =
        getAllNames(model->getDependentVariables());
    TDoubleVector initialConditionValues(initialConditionNames.size());
    i = 0;
    for (const auto &s : initialConditionNames) {
        initialConditionValues[i++] = parameterSet.getParameter(s).getValue();
    }
    solve(model, modelParamValues, initialConditionValues, tout, yout, pvm);
}

/**
 * All the solve() functions eventually call this one. This is here
 * that we will assign to the initial the values defined in the model.
 */

TDoubleVector workingInitialConditionValues;

void XDESolverWrapper::solve(ModelPtr model,
                             const TDoubleVector &modelParameterValues,
                             const TDoubleVector &initialConditionValues,
                             TDoubleVector &tout, TDoubleMatrix &yout, const ParameterValueMap &pvm) {
    // If tspan has not been set explicitly create it from the start time, stop time, interval.
    if (useDefaultTspan) setTSpan();


    // TODO performSimulation probably does not need the model
    // parameter values, since they are ignored anyway by the model
    // object when called from the mathstat code.
    model->processTimepoints(tspan);
    model->setModelParameterValues(modelParameterValues);

    // Some initial conditions values are given, some are computed,
    // and those that are computed may be using macros. The method in
    // the model will know which initial condition in the vector are
    // fixed and which ones need to be computed, and assign the values
    // that need to be computed.

    // initialConditionValues defined as const. Can change the
    // function signature and do away with this copying.
    // TDoubleVector workingInitialConditionValues(initialConditionValues);
    xde_copy(workingInitialConditionValues, initialConditionValues);
    model->initializeInitialConditionValues(workingInitialConditionValues);
    model->initializeTVFunctions();
    // model -> initializeCovariates();

    // cerr << "Will perform simulation: tspan used: " << tspan << endl;
    performSimulation(model, modelParameterValues,
                      workingInitialConditionValues, tout, yout, pvm);
    // cerr << "Done simulation: tout returned: " << tout << endl;
}



/**
 * Estimate the amount of memory used for solving.
 *
 * Will estimate the amount of memory that will be required to solve
 * the equations. The value returned is in bytes. Assume
 */

double XDESolverWrapper::estimateMemoryUse(ModelPtr model) {
    if (model == NULL) return 0;
    // If no tspan has been explicitely defined yet, use the default
    // one.

    if (tspan.size() == 0) setTSpan();
    int nbRows = tspan.size();
    int nbCols = model->getDependentVariables().size();

    return 8.0 * nbRows * nbCols;
}
