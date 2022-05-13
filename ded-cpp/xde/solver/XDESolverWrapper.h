#ifndef XDESOLVERWRAPPER_H_
#define XDESOLVERWRAPPER_H_

#include <algorithms/ConcreteParameters.h>
#include <algorithms/XDEAlgorithm.h>
#include <base/XDEBlas.h>


#include <model/XDEModel.h>
#include <parameters/ParameterSet.h>

#include "SimulationSymbolAttributes.h"
#include "SolverResult.h"

class XDESolverWrapper;

typedef std::shared_ptr<XDESolverWrapper> SolverPtr;
typedef ParameterSet<SimulationSymbolAttributes> SimulationParameterSet;

typedef std::shared_ptr<ParameterSet<SimulationSymbolAttributes> >
    SimulationParameterSetPtr;

/**
 * \brief Base class for all the classes handling the solver wrappers.
 *
 */

class XDESolverWrapper : public XDEAlgorithm {
   public:
    XDESolverWrapper();
    virtual ~XDESolverWrapper();

    // Set tspan based on values for start, end, timestep defined in object.
    void setTSpan();

    // Initialize tspan with the values of newTSpan
    void setTSpan(const TDoubleVector &newTSpan);
    TDoubleVector getTSpan() { return tspan; }

    /**
     * Two options for performing the simulation. In one case returns
     * a vector of timepoints and a matrix of data, and in the other
     * returns a SolverResult object. First method should be used by
     * the estimator, and the second when the results of the
     * simulation will be presented to a user.
     */
    void solve(ModelPtr model, SimulationParameterSet &parameterSet,
               SolverResult &solverResult);
    void solve(ModelPtr model, SimulationParameterSet &parameterSet,
               TDoubleVector &tout, TDoubleMatrix &yout);
    void solve(ModelPtr model, const TDoubleVector &modelParameterValues,
               const TDoubleVector &initialConditionValues, TDoubleVector &tout,
               TDoubleMatrix &yout);

    // Should this one be protected?
    virtual void performSimulation(ModelPtr model,
                                   const TDoubleVector &modelParameterValues,
                                   const TDoubleVector &initialConditionValues,
                                   TDoubleVector &tout,
                                   TDoubleMatrix &yout) = 0;

    static std::vector<std::string> getAttributes();

    virtual double estimateMemoryUse(ModelPtr model);

    // TODO: Get rid of the estimateMemoryUse function.
    virtual double estimateResultingObjectSize(ModelPtr model) {
        return estimateMemoryUse(model);
    }

    double getStartTime() { return startTime; }
    void setStartTime(double d) { startTime = d; }

    double getStopTime() { return stopTime; }
    void setStopTime(double d) { stopTime = d; }

    double getStepSize() { return stepSize; }
    void setStepSize(double d) { stepSize = d; }

   protected:
    TDoubleVector tspan;

    void setModelParasValue(const TDoubleVector &newModelParasValue);
    void setInitCondsValue(const TDoubleVector &newInitCondsValue);

    bool useDefaultTspan;

    double startTime;
    double stopTime;
    double stepSize;
};

#endif /*XDESOLVERWRAPPER_H_*/
