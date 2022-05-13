#ifndef DIFFERENTIAL_EVOLUTION_H
#define DIFFERENTIAL_EVOLUTION_H

#include "XDEOptimizer.h"

/**
 * \brief Pure virtual class describing optimizer based on differential
 * evolution.
 */

class DifferentialEvolution : public XDEOptimizer {
   public:
    DifferentialEvolution();
    virtual ~DifferentialEvolution();

   protected:
    static std::vector<std::string> strategies;

    static std::string getCtrlValueToReach() { return "Value to Reach"; }
    static std::string getCtrlNumberPoints() { return "Number Points"; }
    static std::string getCtrlMaxIterations() { return "Max Iteration"; }

    static std::string getCtrlAmplifyFactor() { return "Amplify Factor"; }

    static std::string getCtrlCrossover() { return "Crossover"; }

    static std::string getCtrlStrategy() { return "Strategy"; }
};

#endif  // DIFFERENTIAL_EVOLUTION_H
