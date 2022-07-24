#include <model/XDEModel.h>
#include <objectfunction/LeastSquare.h>
#include <objectfunction/SolvingDataGenerator.h>
#include <optimizer/AdrianDE.h>
#include <solver/SundialsSolverWrapper.h>
#include <statistics/NLSSummary.h>
#include <unistd.h>

#include <boost/make_shared.hpp>
#include <string>
using namespace std;

static string eqns =
    "\n\
dT/dt = (rho - k_m*T_m - k_w*T_w - k_R*T_mw)*T\n\
dT_m/dt = (rho_m + k_m*T -q_m*T_w)*T_m + p_m*k_R*T_mw*T\n\
dT_w/dt = (rho_w + k_w*T -q_w*T_m)*T_w +  p_m*k_R*T_mw*T\n\
dT_mw/dt = (rho_mw + (1 - p_m -p_w)*k_R*T)*T_mw + (q_m + q_w)*T_w*T_m\n\
p_m=0.25\n\
p_w=0.25";

static ModelPtr modelPtr(std::make_shared<XDEModel>(eqns));
static SolverPtr solver(std::make_shared<SundialsSolverWrapper>());
static ObjectiveFunctionPtr objf(std::make_shared<LeastSquare>());
static ConfidenceIntervalPtr ci(std::make_shared<NLSSummary>());

static EstimationParameterSetPtr createParameterSet() {
    EstimationParameterSetPtr parameterSet(
        std::make_shared<EstimationParameterSet>());

    std::vector<string> names = {"k_R", "k_m",   "k_w",    "q_m",   "q_w",
                                 "rho", "rho_m", "rho_mw", "rho_w", "T",
                                 "T_m", "T_mw",  "T_w"};
    std::vector<double> defaults = {
        4.36e-10, 1.51e-09, 1.11e-09,   4.15e-09, 1.1e-09, 0.015,  -0.0229,
        0.000568, 0.00713,  33300000.0, 130000.0, 9210.0,  25100.0};

    std::vector<double> mins = {0.0,      0.0,    0.0,    0.0,   0.0,
                                -0.06,    -0.2,   -0.2,   -0.06, 10000000.0,
                                100000.0, 4000.0, 10000.0};
    std::vector<double> maxs = {1.0e-08,  1.0e-08, 1.0e-08, 1.0e-08, 1.0e-08,
                                0.06,     0.06,    0.06,    0.06,    50000000.0,
                                300000.0, 20000.0, 60000.0};

    std::vector<bool> estimated = {true, true, true, true, true, true, true,
                                   true, true, true, true, true, true, true};

    for (auto i = 0; i < names.size(); i++) {
        parameterSet->addParameter(EstimationSymbolAttributes(
            names[i], defaults[i], mins[i], maxs[i], estimated[i]));
    }

    return parameterSet;
}

static DataTableMappingPtr createDataTableMapping() {
    vector<double> timepoints = {70, 94, 115, 139, 163};
    vector<string> formulas = {"T",   "T",   "T",   "T_m",  "T_m",  "T_m",
                               "T_w", "T_w", "T_w", "T_mw", "T_mw", "T_mw"};

    vector<double> vdata = {32554830,35855330,32597373,134173,158620,98175,26180,25235,22908,9818,10815,6545,
46645200,48652100,52059000,481950,269500,315000,103950,73500,110250,18900,9800,15750,
64240540,62989640,62362300,1230460,1081920,847210,309260,302680,445900,26320,25760,38220,
65563680,79088100,77218680,9863280,7907900,5576620,3000480,3103100,2117920,1364580,900900,478240,
36366400,47349120,49714560,36545600,24613680,17922240,10281600,15167880,17025120,28806400,22069320,16138080};

    // TODO Generate timepoints, data.
    TDoubleMatrix data(timepoints.size(), formulas.size());

    auto iterMat = data.begin1();
    for (auto iterVec = vdata.begin(); iterVec != vdata.end();
         iterVec += formulas.size(), ++iterMat) {
        std::copy(iterVec, iterVec + formulas.size(), iterMat.begin());
    }

    return std::make_shared<DataTableMapping>(timepoints, formulas, data);
}

int main(int, char **) {
    AdrianDE ade = AdrianDE();

    EstimationParameterSetPtr parameterSet = createParameterSet();
    auto datatable = createDataTableMapping();

    TDoubleMatrix temp_results;
    TDoubleVector bestmem;
    double bestval;
    int nfeval;
    int ndata;

    auto pvm = ParameterValueMap(vector<ParameterValue>{});

    // ade.setParameterValue(AdrianDE::maxGenerations, 25);
    // ade.setParameterValue(AdrianDE::p_populationSize, 10);

    DataGeneratorPtr dg(std::make_shared<SolvingDataGenerator>(
        modelPtr, solver, parameterSet, datatable));
    objf->setDataGenerator(dg);

    auto result = ade.optimize(objf, parameterSet, pvm);

    std::cerr << "Completed test. Best val: " << result->getOptimalValue()
              << std::endl;

    // Compute confidence interval.
    // virtual ConfidenceIntervalResultPtr evaluate(OptimizationResultsPtr
    // optimizerResult,
    //                                            DataTableMappingPtr
    //                                            datatableMapping,
    //                                            EstimationParameterSetPtr
    //                                            parameterSet, ModelPtr model,
    //                                            SolverPtr solver)

    // TODO Real value for last parameter. First figure out which one is needed
    auto ciresult =
        ci->evaluate(result, datatable, parameterSet, modelPtr, solver, {});
    cerr << "Computed CI Result\n";
}
