#ifndef TIMEVARYINGFUNCTIONFACTORY_H
#define TIMEVARYINGFUNCTIONFACTORY_H


#include <algorithms/AlgorithmTemplates.h>

#include <extra_parser_functions/LOCF.h>
#include <extra_parser_functions/FortranSmoothingBSpline.h>
#include <extra_parser_functions/MfcatsSpline.h>
#include <extra_parser_functions/BoostSpline.h>


// mfcatsSpline not well implemented, fsmoothing crashes. 
// typedef AlgorithmFactoryTemplate<TimeVaryingFunction, LOCF, FSmoothingBSpline, MfcatsSpline, BoostSpline> TimeVaryingFunctionFactory;
typedef AlgorithmFactoryTemplate<TimeVaryingFunction, LOCF, BoostSpline> TimeVaryingFunctionFactory;


#endif /* TIMEVARYINGFUNCTIONFACTORY_H */
