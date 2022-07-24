#include "NLSSummary.h"

#include <base/XDEUtil.h>
#include <algorithms/ConcreteParameters.h>
#include <objectfunction/ParameterMapping.h>
#include <objectfunction/SolvingDataGenerator.h>
#include <statistics/nls_summary.h>

#include <algorithm>
#include <boost/cast.hpp>
#include <functional>

#include <cmath>
using namespace std;
using namespace std::placeholders;
//NLSSummary::NLSSummary() {}
//NLSSummary::~NLSSummary() {}

/**
 * Compute the confidence interval. Right now it is assumed that the
 * preProcess method has been called for the objective function, and
 * so this will probably not work for computing the CI directly from
 * an estimation result object.
 *
 * So far this class only makes sense with the SolvingDataGenerator
 *
 * NEW VERSION: Will create the data generator here. It is assumed
 * that:
 *  - The RSS has already been computed correctly and is in the
 *  estimatorResult
 *  -
 */

// TODO AL2020: Use smart pointers?
ConfidenceIntervalResultPtr NLSSummary::evaluate(
    OptimizationResultsPtr optimizationResults, 
    DataTableMappingPtr datatableMapping,
    EstimationParameterSetPtr parameterSet,
    ModelPtr model, SolverPtr solver, const ParameterValueMap &pvm) {
    
    dataGenerator.reset(new SolvingDataGenerator(model, solver, parameterSet,
                                                 datatableMapping));

    dataGenerator->preProcess();

    double alpha = pvm.getDoubleParameterValue(getAlphaParameterName(), 0.5);
    TDoubleVector res;
    TDoubleVector linearData = dataGenerator->getFixedData();
    XDE_ASSERT(linearData.size() > 3);

    std::vector<double> SE;
    std::vector<double> LCB;
    std::vector<double> UCB;
    std::vector<double> tstat;
    std::vector<double> pvals;

    /**
     * Initialize the mapping as is done for the objective function.
     */
    auto estimatedParameters = parameterSet->getParameters(&EstimationSymbolAttributes::isEstimated);
    ParameterMapping::initializeMapping(
        modelParameterValues, initialConditionValues, parameterMapping,
        parameterSet, model);
    auto paramValues = optimizationResults->getParamValues();
    
    std::function<vector<double>(const vector<double>&)> func = std::bind(&NLSSummary::performSimulation, _1, this);


    nls_summary(alpha, paramValues, XDEUtil::getDoubleVector(linearData), optimizationResults->getOptimalValue(), func, 
                SE, LCB, UCB, tstat, pvals);

    vector<string> parameterNames;
    for (const EstimationSymbolAttributes &es : estimatedParameters) {
        parameterNames.push_back(es.getName());
    }
    return std::make_shared<ConfidenceIntervalResult>(optimizationResults, parameterNames, SE, LCB, UCB, pvals, tstat);
}

/*
 * Change the value for the covariance matrix.
 */
// void NLSSummary::setCovarianceMatrix(const TDoubleMatrix &newMatrix) {
//     if (newMatrix.size1() != covarianceMatrix.size1() ||
//         newMatrix.size2() != covarianceMatrix.size2())
//         covarianceMatrix.resize(newMatrix.size1(), newMatrix.size2(), false);
//     for (unsigned i = 0; i < newMatrix.size1(); i++) {
//         for (unsigned j = 0; j < newMatrix.size2(); j++) {
//             covarianceMatrix(i, j) = newMatrix(i, j);
//         }
//     }
// }

/**
 * This function is meant to be called from the mathstat CI routine,
 * and so is static.
 *
 * Compute the simulation with the given values for the parameters, and map the
 * resulting values into the solution vector.
 */
vector<double> NLSSummary::performSimulation(const std::vector<double> &params,
                                    NLSSummary *nlsPtr) {
    TDoubleVector tout;
    TDoubleMatrix yout;

    // Set the parameters provided by mathstat into their correct location.

    unsigned nbParams = params.size();

    for (unsigned i = 0; i < nbParams; i++) {
        *(nlsPtr->parameterMapping[i]) = params[i];
    }

    DataGenerator::DoubleVectorPair dv =
        nlsPtr->getDataGenerator()->generateData(params); // Will call the solver.
    return XDEUtil::getDoubleVector(dv.computed);
}
