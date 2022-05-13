#ifndef OBJECTIVEFUNCTIONFACTORY_H_
#define OBJECTIVEFUNCTIONFACTORY_H_

#include <algorithms/AlgorithmTemplates.h>

#include "LeastSquare.h"


typedef AlgorithmFactoryTemplate<XDEObjectiveFunction, LeastSquare> ObjectiveFunctionFactory;



#endif /* OBJECTIVEFUNCTIONFACTORY_H_ */
