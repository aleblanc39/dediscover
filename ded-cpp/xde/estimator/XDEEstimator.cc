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
