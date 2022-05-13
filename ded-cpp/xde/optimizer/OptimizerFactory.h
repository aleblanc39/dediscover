/*
 * OptimizerFactory.h
 *
 */

#ifndef OPTIMIZERFACTORY_H_
#define OPTIMIZERFACTORY_H_

#include <algorithms/AlgorithmTemplates.h>

#include "TrustRegion.h"
#include "AdrianDE.h"

typedef AlgorithmFactoryTemplate<XDEOptimizer,
    TrustRegion, AdrianDE> OptimizerFactory;



#endif /* OPTIMIZERFACTORY_H_ */
