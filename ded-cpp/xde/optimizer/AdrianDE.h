#ifndef ADRIAN_DE_H
#define ADRIAN_DE_H

#include <algorithms/ConcreteParameters.h>
#include <optimizer/XDEOptimizer.h>

class AdrianDE : public XDEOptimizer {
   public:
    AdrianDE();
    ~AdrianDE() {}

    const static std::string maxGenerations;
    const static std::string crossoverFactor;
    const static std::string p_populationSize;

    const static std::string mutationStrategy;
    const static std::string selectionStrategy;
    const static std::string weightFactor;

    static std::vector<GeneralParameterPtr> getControlParameters() {
        std::vector<int> options = {1, 2, 3, 4, 5};

        return {
            std::make_shared<IntParameter>(
                maxGenerations, 200, 5, 0, true, false,
                "Max number of generations before termination"),

            std::make_shared<IntParameter>(p_populationSize, 50, 5, 0, true,
                                           false, "Size of population"),

            std::make_shared<IntParameter>(mutationStrategy, 1, options, 0, 0,
                                           false, false, "Mutation Strategy: "),
            std::make_shared<DoubleParameter>(crossoverFactor, 0.5, 0.0, 1.0,
                                              true, true,
                                              "The Crossover factor"),
            std::make_shared<DoubleParameter>(weightFactor, 0.5, 0.0, 1.0, true,
                                              true, "The weight factor")};
    }

    std::string getMethodName() { return s_getMethodName(); }
    static std::string s_getMethodName() { return "Adrian DE"; }

    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Differential Evolution code written by Adrian Michel.";
    }

    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }
    static std::string s_getLongMethodDescription() {
        return "Differential Evolution is a stochastic global optimization "
               "method, a simple and efficient heuristic for global "
               "optimization over continuous spaces.";
    }

    static std::vector<std::string> s_getMethodAttributes() {
        return std::vector<std::string>();
    }

   protected:
    OptimizationResultsPtr performOptimization(const ParameterValueMap &pvm);

   private:
};

#endif /**  ADRIAN_DE_H **/