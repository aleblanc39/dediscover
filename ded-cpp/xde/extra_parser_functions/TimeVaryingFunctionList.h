#ifndef TIMEVARYINGFUNCTIONLIST_H
#define TIMEVARYINGFUNCTIONLIST_H

/**
 * Class to handle the Time Varying functions.
 *
 * This class will store a list of time-varying functions that the
 * parser will use to add calls in muParser that will call the
 * functions stored here.
 *
 *
 * Implemented loosely following the singleton pattern.
 */


#include "GenericFunctionList.h"
#include "TimeVaryingFunction.h"

typedef GenericFunctionList <TimeVaryingFunctionPtr> TimeVaryingFunctionList;

#endif
