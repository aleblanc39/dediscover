#include "DifferentialEvolution.h"

#include <algorithms/ConcreteParameters.h>

std::vector<std::string> DifferentialEvolution::strategies;

DifferentialEvolution::DifferentialEvolution() {
    controlParameters.push_back(GeneralParameterPtr(new DoubleParameter(
        getCtrlValueToReach(), 0.0, 0.0, 0.0, false, false,
        "Once the value of objective function is less than or equal to this "
        "value, the optimization procedure stops.")));
    controlParameters.push_back(GeneralParameterPtr(
        new IntParameter(getCtrlNumberPoints(), 300, 4, 0, true, false,
                         "Number of parameter candidates in each interation")));
    controlParameters.push_back(GeneralParameterPtr(
        new IntParameter(getCtrlMaxIterations(), 300, 1, 0, true, false,
                         "Maximum number of iterations")));
    controlParameters.push_back(GeneralParameterPtr(new DoubleParameter(
        getCtrlAmplifyFactor(), 0.8, 0.0, 2.0, true, true,
        "A constant factor between 0 and 2 which controls the amplification of "
        "the distance between two parameter candidates.")));
    controlParameters.push_back(GeneralParameterPtr(
        new DoubleParameter(getCtrlCrossover(), 0.99, 0.0, 1.0, true, true,
                            "crossover probability on [0, 1]")));
    strategies.clear();
    strategies.push_back("Higher Diversity");
    strategies.push_back("Faster Convergency ");
    strategies.push_back("Mix of Above");
    controlParameters.push_back(GeneralParameterPtr(new StringParameter(
        getCtrlStrategy(), strategies[0], strategies, "", "", false, false,
        "1 --> preserve diversity   2 --> improve convergency  3 --> mix of "
        "diversity and convergency")));
}

DifferentialEvolution::~DifferentialEvolution() {}
