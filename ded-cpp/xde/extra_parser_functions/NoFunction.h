#ifndef NOFUNCTION_H
#define NOFUNCTION_H

#include <base/CBIMDefinitions.h>

#include "TimeVaryingFunction.h"

/**
 * \brief No function, to be used as place holder when no function is defined.
 * Wil always return 0
 *
 */

class NoFunction : public TimeVaryingFunction {
   public:
    ~NoFunction(){};

    NoFunction(){};
    std::string getMethodName() {
        return "Should never be seen -- method name";
    }

    std::string getShortMethodDescription() {
        return "Should never be seen...";
    }

    std::string getLongMethodDescription() { return ""; }

    double estimateResultingObjectSize() { return 0; }
    double computeFunction(const double &) { return 0; }

   protected:
    void initializeSpecific(){};

   private:
};

#endif  // NOFUNCTION_H
