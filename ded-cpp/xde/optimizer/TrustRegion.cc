#include "TrustRegion.h"

#include <base/XDEUtil.h>

#include <boost/make_shared.hpp>

extern "C" {
void c_nls_stir(double *x, double *lb, double *ub,
                void f(double *x, double *f, long obj), int sizep, int sizec,
                double tolx, double tolf, int maxiter, long obj);
}

TrustRegion::TrustRegion() {}

TrustRegion::~TrustRegion() {}

OptimizationResultsPtr TrustRegion::performOptimization(const ParameterValueMap&) {
    unsigned nbParams = initialValues.size();
    double *lb = (double *)malloc(nbParams * sizeof(double));
    double *ub = (double *)malloc(nbParams * sizeof(double));
    double *init = (double *)malloc(nbParams * sizeof(double));

    for (unsigned i = 0; i < nbParams; i++) {
        lb[i] = minBounds[i];
        ub[i] = maxBounds[i];
        init[i] = initialValues[i];
    }

    int nbOut = objectiveFunction->getDataGenerator()->getNbOut();
    c_nls_stir(init, lb, ub, &TrustRegion::staticComputeLSTerms, (int)nbParams,
               nbOut, 1e-6, 1e-9, 1000, (long)this);

    // Results will be in init. Should recompute least squares with this values
    // to get bestval. Or modify the fortran code to return it as well.

    // bestmem.resize(nbParams);
    TDoubleVector bestmem(nbParams);
    for (unsigned i = 0; i < nbParams; i++) {
        bestmem(i) = init[i];
    }

    auto bestval = objectiveFunction->computeFunction(bestmem);

    // std::vector<double> dv(nbParams);
    // copy(bestmem.begin(), bestmem.end(), dv.begin());

    return std::make_shared<OptimizationResults>(
        XDEUtil::getDoubleVector(minBounds),
        XDEUtil::getDoubleVector(maxBounds), bestval,
        XDEUtil::getDoubleVector(bestmem),
        objectiveFunction->computeNumObservations());
}

