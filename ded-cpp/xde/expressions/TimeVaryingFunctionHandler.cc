#include "TimeVaryingFunctionHandler.h"

#include <base/XDEException.h>
#include <extra_parser_functions/LOCF.h>
#include <extra_parser_functions/MfcatsSpline.h>
#include <extra_parser_functions/BoostSpline.h>
#include <extra_parser_functions/TimeVaryingFunctionFactory.h>

using namespace std;

static TimeVaryingFunctionFactory factory;

/**
 * Will map how the function is specified in the parser to how they are
 * expected in the factory. Adds a constraint to the parser, but the parser
 * can still add an extra mapping. At least the TVF class names can change
 * without affecting the parser itself.
 *
 * AL 2021: Why did we remove the Fortran option? Not convenient in equations?
 */

std::map<std::string, std::string> TimeVaryingFunctionHandler::TVFunctionMap =

    {{"locf", LOCF::s_getMethodName()},
     {"boost_spline", BoostSpline::s_getMethodName()}
//     {"mfcats_spline", MfcatsSpline::s_getMethodName()}

};

TimeVaryingFunctionPtr TimeVaryingFunctionHandler::createTVFunction(
    std::string functionType, MemorySynchronizerPtr mem,
    std::string originalFunction) {
    // First check that there is a mapping
    if (TVFunctionMap.find(functionType) == TVFunctionMap.end())
        throw XDEException(string("Undefined time-varying function name: ") +
                           functionType);

    // Any exception thrown here should be caught by the caller
    std::cerr << "TimeVaryingFunctionHandler: creating function of type " << functionType << std::endl;
    TimeVaryingFunctionPtr ret(
        factory.createMethod(TVFunctionMap[functionType]));
    ret->setMemorySynchronizer(mem);
    ret->setFunctionString(originalFunction);

    return ret;
}
