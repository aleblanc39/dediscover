
#ifndef TRUST_REGION_H
#define TRUST_REGION_H

#include "XDEOptimizer.h"
#include <base/XDEUtil.h>
#include <cmath>

/**
 * \brief Wrapper around the TrustRegion method implemented
 * in Fortran.
 *
 * [1] Coleman, T.F. and Y. Li, "An Interior, Trust Region Approach for
 *Nonlinear Minimization Subject to Bounds," SIAM Journal on Optimization, Vol.
 *6, pp. 418–445, 1996. [2] Coleman, T.F. and Y. Li, "On the Convergence of
 *Reflective Newton Methods for Large-Scale Nonlinear Minimization Subject to
 *Bounds," Mathematical Programming, Vol. 67, Number 2, pp. 189-224, 1994.
 *
 * Author : P.Linel
 */

class TrustRegion : public XDEOptimizer {
   public:
    TrustRegion();
    virtual ~TrustRegion();

    // Methods from XDEAlgorithm
    static std::string s_getMethodName() { return "Trust Region"; }
    std::string getMethodName() { return s_getMethodName(); }

    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Optimizer implementing the interior point, Trust-region "
               "approach";
    }

    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }
    static std::string s_getLongMethodDescription() {
        return "The Trust-Region method implements the algorithm of Coleman and Li, first published around 1994.\
Note that this method will always use least squares, regardless of what is indicated in the objective function";
    }



    static std::vector<std::string> s_getMethodAttributes() {
		return std::vector<std::string>();
	}



    /**
     * Objective function used in the trust_region fortran
     * method. Since fortran 90 does not really have a concept of null
     * pointer, we send a long.
     */
    static void staticComputeLSTerms(double *parameters, double *fx,
                                     long ptrToObject) {
        TrustRegion *obj = (TrustRegion *)ptrToObject;
        auto nbTerms = obj->getObjectiveFunction()->computeLSTerms(parameters, fx);

        // TrustRegion will crash if there are NaNs in fx, sometimes as a result of
        // taking the log of a negative value.

        for (auto i = 0; i < nbTerms; i++) {
            if (std::isnan(fx[i])) {
                std::cerr << "Have a nan\n";
                //fx[i] = XDEUtil::XDE_INF;
                fx[i] = 100000;

            }
        }
    }



    double estimateResultingObjectSize(){return 0;}

   protected:
    virtual OptimizationResultsPtr performOptimization();
};

#endif // TRUST_REGION_H