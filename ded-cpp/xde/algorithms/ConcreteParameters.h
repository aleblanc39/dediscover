#ifndef CONCRETEPARAMETERS_H
#define CONCRETEPARAMETERS_H

#include <algorithms/GenericParameter.h>
#include <string>

/**
 * Implementations of the GenericParameter template for the types that
 * are used by DEDiscover.
 */ 

typedef GenericParameter<double> DoubleParameter;
typedef GenericParameter<int> IntParameter;
typedef GenericParameter<bool> BoolParameter;
typedef GenericParameter<std::string> StringParameter;

#endif
