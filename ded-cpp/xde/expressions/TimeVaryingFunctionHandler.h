#ifndef TIMEVARYINGFUNCTIONHANDLER_H
#define TIMEVARYINGFUNCTIONHANDLER_H

#include <string>

#include <extra_parser_functions/TimeVaryingFunction.h>
#include "MemorySynchronizer.h"


/**
 * \brief Class to handle a mapping between the name of a function in the XDE equations
 * and the corresponding classes in the XDE code.
 *
 * Creating this mapping allows us to easily modify either the ways functions are
 * named in the equations or the names of the classes for the specific function
 * without affecting the rest of the code.
 */
class TimeVaryingFunctionHandler {

 public:
    static TimeVaryingFunctionPtr createTVFunction(std::string functionType = "locf",
						   MemorySynchronizerPtr mem = MemorySynchronizerPtr(),
						   std::string originalFunction = "");

    static bool isFunctionDefined(std::string fcnName) {
	return !(TVFunctionMap.find(fcnName) == TVFunctionMap.end());
    }

 private:
    static std::map<std::string, std::string> TVFunctionMap;
};



#endif //TIMEVARYINGFUNCTIONHANDLER_H

