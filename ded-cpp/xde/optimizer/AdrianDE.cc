#include <base/XDEMessage.h>
#include <base/XDEUtil.h>
#include <math.h>
#include <optimizer/AdrianDE.h>
#include <optimizer/OptimizationResults.h>

#include <chrono>  // std::chrono::milliseconds
#include <future>  // std::async, std::future
#include <optimizer/differentialevolution/differential_evolution.hpp>
#include <string>
#include <thread>

using namespace amichel;
using namespace std;

const std::string AdrianDE::maxGenerations = "Max Generations";
const std::string AdrianDE::crossoverFactor = "Crossover Factor";
const std::string AdrianDE::p_populationSize = "Population Size";

const std::string AdrianDE::mutationStrategy = "Mutation Strategy";
const std::string AdrianDE::selectionStrategy = "Population Size";
const std::string AdrianDE::weightFactor = "Weight";

// static string populationSize = "Population Size";
// static string populationSize = "Population Size";

#define MAKE_MUTATION_STRATEGY(n)                \
    std::make_shared<de::mutation_strategy_##n>( \
        minBounds.size(), de::mutation_strategy_arguments(weight, crossover))

AdrianDE::AdrianDE() {
  
}

class AdObjectiveFunction {
    ObjectiveFunctionPtr xdeObjFcn;
    int nbCalls = 0;
    TDoubleVector params;

   public:
    AdObjectiveFunction(ObjectiveFunctionPtr o, unsigned nparams) {
        xdeObjFcn = o;
        params.resize(nparams);
    }

    double operator()(const de::DVector &args) {
        std::copy(args.begin(), args.end(), params.begin());

        try {
            double d = xdeObjFcn->computeFunction(params);
            return (isnan(d)) ? XDEUtil::XDE_INF : d;
        } catch (...) {
            // An unhandled exception would kill the whole process
            return XDEUtil::XDE_INF;
        }
    }
};

void run_optimization(de::differential_evolution &diffeval, bool &done) {
    diffeval.run();
    done = true;
}

OptimizationResultsPtr AdrianDE::performOptimization(const ParameterValueMap &pvm) {
    de::constraints constraints(minBounds.size(), -1.0e6, 1.0e6);

    for (int i = 0; i < minBounds.size(); i++) {
        constraints[i] =
            std::make_shared<de::real_constraint>(minBounds[i], maxBounds[i]);
    }
    de::ObjectiveFunction of(
        AdObjectiveFunction(objectiveFunction, minBounds.size()));
    de::listener_ptr listener(std::make_shared<de::null_listener>());
    de::processor_listener_ptr processor_listener(
        std::make_shared<de::null_processor_listener>());

    // Not ready yet for multi threaded.
    de::processors processors(1, of, processor_listener);

    // Control Parameters

    auto termObject = de::max_gen_termination_strategy{
        pvm.getIntParameterValue(maxGenerations)};

    
    de::termination_strategy ts(termObject);

    de::selection_strategy selectionStrategy{
        de::best_parent_child_selection_strategy{}};

    de::mutation_strategy_ptr _mutationStrategy;

    double weight = pvm.getDoubleParameterValue(weightFactor);
    double crossover = pvm.getDoubleParameterValue(crossoverFactor);
    switch (pvm.getIntParameterValue(mutationStrategy)) {
        case 1:
            _mutationStrategy = MAKE_MUTATION_STRATEGY(1);
            break;
        case 2:
            _mutationStrategy = MAKE_MUTATION_STRATEGY(2);
            break;
        case 3:
            _mutationStrategy = MAKE_MUTATION_STRATEGY(3);
            break;
        case 4:
            _mutationStrategy = MAKE_MUTATION_STRATEGY(4);
            break;
        case 5:
            _mutationStrategy = MAKE_MUTATION_STRATEGY(5);
            break;
        default:
            throw XDEException(
                (boost::format("Invalid mutation strategy: %1%") %
                 _mutationStrategy)
                    .str());
    }

    // de::mutation_strategy_arguments mutation_arguments(0.5, 0.9);
    de::mutation_strategy_ptr mutationStrategy(_mutationStrategy);

    // de::mutation_strategy_ptr mutationStrategy(
    //     std::make_shared<de::mutation_strategy_1>(minBounds.size(),
    //                                               mutation_arguments));

    bool minimize = true;
    unsigned populationSize = pvm.getIntParameterValue(p_populationSize);

    de::differential_evolution diffeval(
        minBounds.size(), populationSize, processors, constraints, minimize, ts,
        selectionStrategy, mutationStrategy, listener);

    // run the optimization process
    try {
        bool done = false;
        std::thread optimization_thread (run_optimization, std::ref(diffeval), std::ref(done));
        while(!done) {
            std::cerr << ".";
            _SLEEP(1);
            if (threadStatus == INTERRUPT) {
                std::cerr << "AdrianDE: Killing thread\n";
                diffeval.force_terminate = true;
            }
        }
         std::cerr << "\nCompleted job\n";
         optimization_thread.join();

    } catch (...) {
        std::cerr << "We caught something!!!\n";
        return std::make_shared<OptimizationResults>();
    }
    auto best = diffeval.best();

    return std::make_shared<OptimizationResults>(
        XDEUtil::getDoubleVector(minBounds),
        XDEUtil::getDoubleVector(maxBounds), best->cost(), best->vars(),
        objectiveFunction->computeNumObservations());
}