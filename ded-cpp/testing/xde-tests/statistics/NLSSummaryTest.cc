#include <doctest.h>
#include <optimizer/OptimizationResults.h>
#include <statistics/NLSSummary.h>
#include <solver/SundialsSolverWrapper.h>


#include <base/XDEUtil.h>
#include <iostream>

using namespace std;

static string eqns =
    "\n\
dPrey/dt = (birth - predation * Predator) * Prey\n\
dPredator/dt = (growth * Prey - death) * Predator\n";

static DataTableMappingPtr createDataTableMapping() {
    vector<double> timepoints = {1911,1913,1915,1917,1919,1921,1923,1925,1927,1929,1931,1933,1935,1937};
    vector<string> formulas = {"Prey", "Predator"};
    vector<double> vdata = {
27,	7,
77,	20,
25,	43,
10,	11,
10,	6,
46,	20,
80,	37,
20,	43,
8, 50,
6, 30,
6, 15,
20, 18,
83,	40,
12,	48};
    // TODO Generate timepoints, data.
    TDoubleMatrix data(timepoints.size(), formulas.size());

    auto iterMat = data.begin1();
    for (auto iterVec = vdata.begin(); iterVec != vdata.end(); iterVec += formulas.size(), ++iterMat) {
        std::copy(iterVec, iterVec+formulas.size(), iterMat.begin());
    }

    return std::make_shared<DataTableMapping>(timepoints, formulas, data);
}






SCENARIO("Testing straightLineInterploation") {
    GIVEN("A made upoptimization result") {
        vector<double> lb = {0, 0, 1, 1};
        vector<double> ub = {4, 5, 1.6, 1.9};

        vector<double> optimalValues = {4.0, 0.75, 0.25, 2.0};
        double optimalValue = 15;
        double nobs = 24;

        auto model = std::make_shared<XDEModel>(eqns);

        NLSSummary nls = NLSSummary();
        auto optimizationResults = std::make_shared<OptimizationResults>(
            lb, ub, optimalValue, optimalValues, nobs);

        // Validity of results not an issue here. 

        auto paramNames = getAllNames(model->getModelParameters());
        auto solver = std::make_shared<SundialsSolverWrapper>();
        auto parameterSet = std::make_shared<EstimationParameterSet>();
            
        parameterSet -> addParameter(EstimationSymbolAttributes("birth", 0, 0, 10, true));
        parameterSet -> addParameter(EstimationSymbolAttributes("predation", 0,0, 10, true));
        parameterSet -> addParameter(EstimationSymbolAttributes("growth", 0, 0, 10, true));
        parameterSet -> addParameter(EstimationSymbolAttributes("death", 0, 0, 10, true));
        parameterSet -> addParameter(EstimationSymbolAttributes("Predator", 7, 0, 100, false));
        parameterSet -> addParameter(EstimationSymbolAttributes("Prey", 27, 0, 100, false));

        auto ciResults = nls.evaluate(optimizationResults, createDataTableMapping(), parameterSet, model, solver);
        THEN("We didn't crash") { CHECK(ciResults->getLowerBounds().size() == 4); }

    }
}