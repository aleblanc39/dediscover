#include <model/XDEModel.h>
#include <objectfunction/LeastSquare.h>
#include <objectfunction/SolvingDataGenerator.h>
#include <optimizer/TrustRegion.h>
#include <solver/SundialsSolverWrapper.h>
#include <statistics/NLSSummary.h>
#include <unistd.h>

#include <string>

#include <boost/make_shared.hpp>
using namespace std;

static string eqns =
    "\n\
dPrey/dt = (birth - predation * Predator) * Prey\n\
dPredator/dt = (growth * Prey - death) * Predator\n";

static ModelPtr modelPtr(std::make_shared<XDEModel>(eqns));
static SolverPtr solver(std::make_shared<SundialsSolverWrapper>());
static ObjectiveFunctionPtr objf(std::make_shared<LeastSquare>());
static ConfidenceIntervalPtr ci(std::make_shared<NLSSummary>());


static EstimationParameterSetPtr createParameterSet() {
    EstimationParameterSetPtr parameterSet(std::make_shared<EstimationParameterSet>());
    parameterSet -> addParameter(EstimationSymbolAttributes("birth", 0.5, 0.0, 100.0, true));
    parameterSet -> addParameter(EstimationSymbolAttributes("predation", 0.5, 0.0, 100.0, true));
    parameterSet -> addParameter(EstimationSymbolAttributes("growth", 0.5, 0.0, 100.0, true));
    parameterSet -> addParameter(EstimationSymbolAttributes("death", 0.5, 0.0, 100.0, true));

    parameterSet -> addParameter(EstimationSymbolAttributes("Prey", 27, 0.0, 100.0, false));
    parameterSet -> addParameter(EstimationSymbolAttributes("Predator", 7, 0.0, 100.0, false));

    return parameterSet;
}

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


int main(int, char **) {
    TrustRegion ade = TrustRegion();

    EstimationParameterSetPtr parameterSet = createParameterSet();
    auto datatable =  createDataTableMapping();

    TDoubleMatrix temp_results;
    TDoubleVector bestmem;
    double bestval;


    DataGeneratorPtr dg(std::make_shared<SolvingDataGenerator>(modelPtr, solver, parameterSet, datatable));
    objf -> setDataGenerator(dg);
    std::cerr << "Will log values\n";
    objf -> setParameterValue(LeastSquare::getLogValuesParameterName(), true);

    auto result = ade.optimize(objf, parameterSet);

    std::cerr << "Completed test. Best val: " << result -> getOptimalValue() << std::endl;

    
    auto ciresult = ci -> evaluate(result, datatable, parameterSet, modelPtr, solver);
    cerr << "Computed CI Result\n";

}
