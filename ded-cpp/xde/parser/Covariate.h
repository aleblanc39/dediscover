#ifndef COVARIATE_H
#define COVARIATE_H

#include <extra_parser_functions/TVFunctionMapping.h>
#include <extra_parser_functions/TimeVaryingFunction.h>
#include <extra_parser_functions/TimeVaryingFunctionList.h>

#include "IntermediateSymbol.h"

/**
 * \brief Class to handle covariates defined in the model.
 *
 * This object is created during the parsing of the model,
 * but handles a lot of values assigned during the execution. The
 * responsibilities should be divided. But in the meantime:
 * Will contain the time-varying function that is used when the
 * covariate is evaluated, as well as TVFunctionMapping object, which
 * itelf contains the datatable and a Datatable and a
 * TimeColumnInterface (currently a MappingInfoSet) object.
 *
 * This object must be assigned prior to simulation or estimation, as
 * the parser does not have this information.
 */

class Covariate : public IntermediateSymbol {
   public:
    Covariate(std::string covariateName, unsigned lineNO, int occurenceIndex,
              TimeVaryingFunctionPtr defaultFunction);

    auto getTimeVaryingIndex() { return timeVaryingIndex; }

    std::string getDisplayExpression() { return displayExpression; }
    void setDisplayExpression(const std::string &s) { displayExpression = s; }

    void assignTVFunction(TimeVaryingFunctionPtr p) {
        assignedFunction = p;
        TimeVaryingFunctionList::getInstance()->replaceFunction(
            timeVaryingIndex, p);
        mapped = true;
    }

    auto getTVFunction() {
        if (assignedFunction == NULL) return defaultFunction;
        return assignedFunction;
    }

    void unmapFunction() {
        TimeVaryingFunctionList::getInstance()->replaceFunction(
            timeVaryingIndex, defaultFunction);
        mapped = false;
    }

    void mapFunction() { mapped = true; }

    auto isMapped() { return mapped; }

    
   private:
    bool mapped;

    /**
     * Index in a vector that saves all the time varying functions defined in
     * the mode.
     */

    unsigned timeVaryingIndex;
    std::string displayExpression;

    /**
     * Function mapped to this covariate, and used when mapped is true.
     */
    TimeVaryingFunctionPtr assignedFunction;

    /**
     * Function to use if this covariate is not mapped (i.e. mapped set to
     * false. assignedFunction * can still have a non-null value.)  Usually a
     * function returning 0 on any input, bust does not have to be.
     **/

    TimeVaryingFunctionPtr defaultFunction;
};

typedef std::shared_ptr<Covariate> CovariatePtr;

#endif
