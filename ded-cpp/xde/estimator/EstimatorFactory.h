#ifndef ESTIMATORFACTORY_H_
#define ESTIMATORFACTORY_H_

#include <algorithms/AlgorithmTemplates.h>

#include <estimator/SolverBasedEstimator.h>


typedef AlgorithmFactoryTemplate<XDEEstimator, SolverBasedEstimator> EstimatorFactory;


#endif /* ESTIMATORFACTORY_H_ */
