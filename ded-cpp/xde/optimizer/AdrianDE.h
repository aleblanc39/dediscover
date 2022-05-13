#ifndef ADRIAN_DE_H
#define ADRIAN_DE_H

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
    OptimizationResultsPtr performOptimization();

   private:
};

#endif /**  ADRIAN_DE_H **/