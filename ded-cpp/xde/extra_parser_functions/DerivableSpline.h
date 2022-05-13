#ifndef DERIVABLESPLINE_H
#define DERIVABLESPLINE_H

#include "TimeVaryingFunction.h"

class DerivableSpline;
typedef std::shared_ptr<DerivableSpline> DerivableSplinePtr;

/**
 * \brief Pure virtual base class for the splines classes that
 * support computation of the derivative.
 */

class DerivableSpline: public TimeVaryingFunction {

 public:
    DerivableSpline(std::string fs) : TimeVaryingFunction(fs) {}
	DerivableSpline(std::string fs, MemorySynchronizerPtr mem) : TimeVaryingFunction(fs,  mem) {}
    DerivableSpline(){}
    virtual ~DerivableSpline(){}

    
    virtual double computeDerivative(double t) = 0;

};

#endif
