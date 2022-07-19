#include "estimator/SolverBasedEstimator.h"

#include <cmath>
#include <base/StopThread.h>
#include <base/XDEUtil.h>
#include <extra_parser_functions/TVFunctionException.h>
#include <mapping/DataTableMapping.h>
#include <objectfunction/SolvingDataGenerator.h>

using namespace std;


 double SolverBasedEstimator::computeRSS(OptimizationResultsPtr optimizationResult, EstimationParameterSetPtr estimationParamSet) {
     
    DataGeneratorPtr dataGenerator(
            new SolvingDataGenerator(model, solver, estimationParamSet, mapping));


    DataGenerator::DoubleVectorPair results = dataGenerator->generateData(optimizationResult -> getParamValues());
    double ret = 0;
    for (auto i = 0; i < results.size(); i++) {
        ret += pow((results.observed[i] - results.computed[i]), 2);
    }
    return ret;
}


SolverBasedEstimator::SolverBasedEstimator(ModelPtr model, SolverPtr s,
                                           ObjectiveFunctionPtr o,
                                           OptimizerPtr opt,
                                           DataTableMappingPtr m)
    : XDEEstimator(model, s, o, opt, m) {}

EstimatorResultPtr SolverBasedEstimator::estimate(
    EstimationParameterSetPtr parameterSet) {
        // TODO Have the optimizer fill in the names in the result objects.
        // Shouldn't have to be done here.
        vector<string> names = parameterSet->getParameterNames(
            &EstimationSymbolAttributes::isEstimated);

        model->initializeCovariates();
        vector<EstimationSymbolAttributes> params =
            parameterSet->getParameters();

        DataGeneratorPtr dataGenerator(
            new SolvingDataGenerator(model, solver, parameterSet, mapping));

        // Preprocess the datagenerator so as to know number of data points.
        dataGenerator->preProcess();
        if (dataGenerator->numObservations() <= names.size()) {
            cerr << "Estimation will fail: " << dataGenerator->numObservations()
                 << ", " << names.size() << endl;

            throw XDEException(
                "There are not enough data points for the number of parameters "
                "to "
                "be estimated. Number of data points must be greater than "
                "the number "
                "of parameters.");
        }
        objectiveFunction->setDataGenerator(dataGenerator);

        logAlgorithmInfo();
        logParameterInfo(parameterSet);
        
        try {
            auto optimizerResult =
                optimizer->optimize(objectiveFunction, parameterSet);

            auto rss = computeRSS(optimizerResult, parameterSet);

            return std::make_shared<EstimatorResult>(optimizerResult, names, rss);

        } catch (...) {
            throw XDEException(
                "Problem occurred when performing the estimation.");
        }
    
}

/**
 * Will add the space used by the optimizer plus the space used by the
 * confidence interval.
 */

double SolverBasedEstimator::estimateResultingObjectSize() {
    XDE_ASSERT(optimizer != NULL);
    return optimizer->estimateResultingObjectSize();
}
