#include <estimator/XDEEstimator.h>
#include <optimizer/AdrianDE.h>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
using namespace std;

OptimizerFactory XDEEstimator::optimizerFactory;

XDEEstimator::~XDEEstimator() {}

XDEEstimator::XDEEstimator(const XDEEstimator &e) : XDEAlgorithm(e) {
    solver = e.getSolver();
    optimizer = e.getOptimizer();
    objectiveFunction = e.getObjectiveFunction();
}

/**
 * Create a default parameterSet for estimation. Will use all model
 * parameters and initial conditions from model. By default model
 * parameters are estimated, initial conditions are not.
 * 
 * TODO: Currently only used in TestNLSSummary. Should remove.
 */

EstimationParameterSetPtr XDEEstimator::createDefaultParameterSet(
    ModelPtr model, bool estimateDepVars) {
    EstimationParameterSetPtr ps(new EstimationParameterSet());

     for (ModelParameterPtr p:
                   model->getModelParameters()) {
        ps->addParameter(
            EstimationSymbolAttributes(p->getName(),
                                       0.5,  // p->getDefaultValue(),
                                       0,    // p->getDefaultMin(),
                                       1,    // p->getDefaultMax(),
                                       true));
    }

    for (DependentVariablePtr p :
                   model->getDependentVariables()) {
        ps->addParameter(
            EstimationSymbolAttributes(p->getName(),
                                       0.5,  // p->getDefaultValue(),
                                       0,    // p->getDefaultMin(),
                                       1,    // p->getDefaultMax(),
                                       estimateDepVars));
    }
    return ps;
}

// Set default optimizer here. Maybe should be somewhere else.
OptimizerPtr XDEEstimator::getOptimizer() {
    if (optimizer == NULL) {
        optimizer.reset(
            optimizerFactory.createMethod(AdrianDE::s_getMethodName()));
    }
    return optimizer;
}

void XDEEstimator::logParameterInfo(EstimationParameterSetPtr ps,
                                    XDEMessage::MsgLevel level) {
    vector<EstimationSymbolAttributes> params = ps->getParameters();
    XDEMessage::log(level, "Initial parameter values for estimation:");
    for (EstimationSymbolAttributes &p : params) {
        XDEMessage::log(
            level,
            "name: " + p.getName() +
                "  min: " + boost::lexical_cast<string>(p.getMinValue()) +
                "  max: " + boost::lexical_cast<string>(p.getMaxValue()) +
                "  initial: " +
                boost::lexical_cast<string>(p.getInitialValue()) +
                "  estimated: " + (p.isEstimated() ? "yes" : "no"));
    }
}
