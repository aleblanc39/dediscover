#include "SolvingDataGenerator.h"

#include <algorithms/ConcreteParameters.h>

#include <algorithm>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <mapping/DataTableMapping.h>

using namespace std;
namespace bacc = boost::accumulators;

/**
 * Only constructor for the class. When generateData is called the
 * estimated parameter values will be listed in a predefined
 * order. During construction must establish a mapping between the
 * location of the values in the argument of generateData and the
 * vectors that are passe to the solver.solve() method.
 *
 * This function is likely to be called many times it is better
 * not to call the version of solver.solve() that used parameter set,
 * but instead the one that takes a vector for the initial conditions,
 * and one for the model parameters.
 */

/**
 * Declare as static for now. May redefine as class variables when we start
 * work on parallelization.
 */
static TDoubleVector tout;
static TDoubleMatrix yout;

static TDoubleVector workTout;
static TDoubleMatrix workYyout;

// AL2020: Why the applyLog, ScalingMode params? Doesn't look like they're used
SolvingDataGenerator::SolvingDataGenerator(ModelPtr model,
                                           SolverPtr s,
                                           EstimationParameterSetPtr e,
                                           DataTableMappingPtr m,
                                           bool applyLog, ScalingMode scaling)
    : DataGenerator(model, e, m), solver(s) {
    preProcess(applyLog, scaling);
}

// Should rename to something indicating that it will fill up the observed data
// array.

void SolvingDataGenerator::preprocessSpecific() {
    workingData.reset(new NewWorkingData(model, mapping));

    if (workingData == NULL || workingData->getUniqueTimepoints().size() == 0)
        throw XDEException(
            "No usable data or timepoints in the datatable to compute the "
            "objective function.");
    solver->setTSpan(workingData->getUniqueTimepoints());

    XDE_ASSERT(model.get() != NULL);
    model->initializeConstantTVFunctions();
    model->initializeCovariates();
    returnedPairs.observed = workingData->getLinearData();
}

void SolvingDataGenerator::preprocessSpecific(
    const std::vector<GeneralParameterPtr> &) {
    XDE_ASSERT(0);
}

SolvingDataGenerator::~SolvingDataGenerator() {}

DataGenerator::DoubleVectorPair SolvingDataGenerator::generateData(
    const std::vector<double> &parameters) {
    unsigned nbParameters = parameters.size();

    XDE_ASSERT(nbParameters == parameterMapping.size());
    for (unsigned i = 0; i < nbParameters; i++) {
        *(parameterMapping[i]) = parameters[i];
    }

    // std::cerr << "Will perform simulation -- model params, init conds:\n";
    // for (auto x: modelParameterValues) std::cerr << x << ", ";
    // std::cerr << std::endl;
    // for (auto x: initialConditionValues) std::cerr << x << ", ";
    // std::cerr << std::endl;
    solver->solve(model, modelParameterValues, initialConditionValues, tout,
                  yout);
    XDE_ASSERT(workingData != NULL);
    // std::cerr << "yout\n";
    // for (unsigned i = 0; i < yout.size1(); i++) {
    //     for (unsigned j = 0; j < yout.size2(); j++)
    //         std::cerr << yout(i, j) << ", ";
    //     std::cerr << std::endl;
    // }
    workingData->getLinearMappedData(yout, returnedPairs.computed);

    // Scale before taking the log, if required.
    if (multiplier.size() > 0) {
        for (unsigned i = 0; i < returnedPairs.computed.size(); i++)
            returnedPairs.computed[i] *= multiplier[i];
    }

    transform(returnedPairs.computed.begin(), returnedPairs.computed.end(),
              returnedPairs.computed.begin(),
              bind1st(mem_fun(DataGenerator::transformData), this));
    XDE_ASSERT(returnedPairs.computed.size() == returnedPairs.observed.size());
    return returnedPairs;
}

/**
 * Fill up the array with the values that will scale the data and
 * observations if the scaling is other than none.
 */
void SolvingDataGenerator::computeMultiplier(ScalingMode scaling) {
    unsigned nbMappedCols = mapping->getColumnData().size();
    TDoubleVector linearData = workingData->getLinearData();
    NewWorkingData::LinearToMatrixMap dataMap = workingData->getDataMap();

    // Put all the data into accumulators.

    typedef pair<unsigned, unsigned> MapEntry;
    vector<bacc::accumulator_set<
        double, bacc::stats<bacc::tag::variance(bacc::lazy)> > >
        accumulators(nbMappedCols);
    unsigned cntr = 0;
    for (MapEntry &me : dataMap) {
        XDE_ASSERT(me.second < nbMappedCols);
        accumulators[me.second](linearData[cntr++]);
    }
    // Put the correct variance or sd in each entry.

    cntr = 0;
    multiplier.resize(dataMap.size());
    for  (MapEntry &me : dataMap) {
        multiplier[cntr++] = bacc::variance(accumulators[me.second]);
    }

    if (scaling == LINEAR) {
        transform(multiplier.begin(), multiplier.end(), multiplier.begin(),
                  (double (*)(double))sqrt);
    }
}
