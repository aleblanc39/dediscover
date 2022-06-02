#include "objectfunction/XDEObjectiveFunction.h"

#include <algorithms/ConcreteParameters.h>
#include <extra_parser_functions/TVFunctionException.h>
#include <objectfunction/ParameterMapping.h>
#include <parser/DependentVariable.h>
#include <parser/ModelParameter.h>

#include <boost/bind.hpp>
#include <cmath>

using namespace std;

XDEObjectiveFunction::~XDEObjectiveFunction() {}

double XDEObjectiveFunction::computeFunction(const TDoubleVector &paras) {
    try {
        double d = performComputation(paras);
        //std::cerr << "computeFunction returns " << d << std::endl;
        return d;
    } catch (TVFunctionException) {
        std::cerr << "Returning NaN\n";
        return XDEUtil::XDE_NAN;
    }
}

unsigned XDEObjectiveFunction::computeLSTerms(double *parameters, double *fx) {
    unsigned nbParams = dataGenerator->getNbParameters();
    unsigned nbOut = dataGenerator->getNbOut();

    std::vector<double> p(nbParams);
    // Pray that parameters is of the right size.
    for (unsigned i = 0; i < nbParams; i++) {
        p[i] = parameters[i];
    }

    DataGenerator::DoubleVectorPair values = dataGenerator->generateData(p);
    for (unsigned i = 0; i < nbOut; i++) {
        fx[i] = values.computed[i] - values.observed[i];
    }
    return nbOut;
}

// What is this for???
// The only effect is to set a value to ndata
// Parameters is the vector of values computed (returned) by the optimizer.
//
// ndata is later used a nobs in the computation of some statistice at the end
// of the estimation.
//
unsigned XDEObjectiveFunction::computeNumObservations() {
    return dataGenerator->numObservations();
}

/**
 * Any reason for not having the computeSum code here?
 */

double XDEObjectiveFunction::performComputation(const TDoubleVector &paras) {
    XDE_ASSERT(paras.size() > 0);
    return computeSum(paras);
}

/**
 * Will do the actual least square computation. The trans argument is
 * the transformation to be applied, usually some sort of log
 * function, but could be the identity function.
 */

double XDEObjectiveFunction::computeSum(const TDoubleVector &parameters) {
    if (!((nbCalls++) % 10000))
        XDEMessage::log(XDEMessage::INFO,
                        string("Objective function call #") +
                            boost::lexical_cast<string>(nbCalls));

    if (find_if(parameters.begin(), parameters.end(), [](double d) { return std::isnan(d); }) !=
        parameters.end()) {
        XDEMessage::log(XDEMessage::WARNING,
                        "One of the parameters is NAN for objective function. "
                        "Returning NAN.");
        return XDEUtil::XDE_NAN;
    }

    DataGenerator::DoubleVectorPair values =
        dataGenerator->generateData(XDEUtil::getDoubleVector(parameters));

    // TODO Have the data generators makes sure there are no values returned
    // outside of the valid range.
    double result = 0;
    unsigned nbValues = values.size();

    // Compute the sum of squares. If a computed value NaN then return NaN. If
    // an observed value is NaN the skip it. 

    // TODO As 2021 Clean up the isNaN mess. Using two different version here
    for (unsigned i = 0; i < nbValues; i++) {
        if (!std::isnan(values.observed[i])) {
            if (!std::isnan(values.computed[i]) &&
                abs(values.observed[i]) != XDEUtil::XDE_INF &&
                abs(values.computed[i]) != XDEUtil::XDE_INF) {
 
                // double z = (*trans)(values.observed[i]) -
                // (*trans)(values.computed[i]);
                double z = (values.observed[i] - values.computed[i]);
                result += z * z;
            } else {
                return XDEUtil::XDE_NAN;
            }
        }
    }

    return result;
}
