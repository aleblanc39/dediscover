#ifndef LOCF_H
#define LOCF_H

#include <base/CBIMDefinitions.h>

#include "TimeVaryingFunction.h"

/**
 * \brief Implementation of the Last Observation Carried Forward method.
 *
 */

class LOCF : public TimeVaryingFunction {
   public:
    virtual ~LOCF();

    LOCF();
    LOCF(MemorySynchronizerPtr mem);
    LOCF(std::string functionString);
    LOCF(std::string functionString, MemorySynchronizerPtr mem);

    static std::string s_getMethodName() { return "LOCF"; }
    std::string getMethodName() { return s_getMethodName(); }
   
    std::string getShortMethodDescription() {
        return s_getShortMethodDescription();
    }
    static std::string s_getShortMethodDescription() {
        return "Last Observation Carried Forward method";
    }


    std::string getLongMethodDescription() {
        return s_getLongMethodDescription();
    }

    static std::string s_getLongMethodDescription() {
        return "Last Observation Carried \
Forward. Given a sequence of time-value pairs (t_1, v_1), (t_2, v_2), ..., (t_n, v_n), sorted in increasing order of time, \
and a time t, return the value v_k such that t_k <= t < t_(k+1). Returns NaN or a used-defined value for out of bounds if \
t< t_1 or t > t_n";
    }

    static std::vector<std::string> s_getMethodAttributes() {
        return std::vector<std::string>();
    }

    double estimateResultingObjectSize() { return 0; }
    double computeFunction(const double &t);


   protected:
    virtual void initializeSpecific();

   private:
    std::vector<std::pair<double, double>> sortedTimeValues;
};

#endif
