#ifndef DERIVABLESPLINEFACTORY_H
#define DERIVABLESPLINEFACTORY_H



#include <algorithms/AlgorithmTemplates.h>

#include <extra_parser_functions/FortranSmoothingBSpline.h>


typedef AlgorithmFactoryTemplate<DerivableSpline, FSmoothingBSpline> DerivableSplineFactory;


#endif /* TIMEVARYINGFUNCTIONFACTORY_H */
