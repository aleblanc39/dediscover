/**
 * Implementation methods for JEstimator class.
 */

#include <estimator/EstimatorFactory.h>
#include <estimator/SolverBasedEstimator.h>
#include <extra_parser_functions/FortranSmoothingBSpline.h>
#include <jni.h>
#include <objectfunction/SolvingDataGenerator.h>
#include <statistics/EstimatorResult.h>

#include "JNIUtil.h"
#include "edu_rochester_urmc_cbim_jni_JEstimator.h"

static EstimatorFactory estimatorFactory;

using namespace std;


/*
 * Class:     edu_rochester_urmc_cbim_jni_JEstimator
 * Method:    nat_estimate
 * Signature:
 *
 * ([Ledu/rochester/urmc/cbim/jni/ParameterValue;Ledu/rochester/urmc/cbim/jni/JDDEModel;)Ledu/rochester/urmc/cbim/jni/EstimatorResult;
 *
 * Performs the estimation. It will uses the Java JNIParameterSet to
 * extract the parameter and create the XDE version of this same
 * parameter set.

 */
JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JEstimator_nat_1estimate(
    JNIEnv *env, jobject obj, jobjectArray parameters) {
    try {
        assert(parameters != NULL);

        EstimatorPtr estimator =
            boost::any_cast<EstimatorPtr>(getNativePointer(env, obj));

        EstimationParameterSetPtr parameterSet =
            createEstimationParameterSet(env, parameters);

        std::cerr << "Starting estimation\n";
        EstimatorResultPtr result = estimator->estimate(parameterSet);

        std::cerr << "Estimation completed. Size of best result: " << result -> getBestResult().size() << std::endl;

        jobject jEstimate = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/EstimatorResult", result);

        return jEstimate;
    } catch (StopThread ex) {
        std::cerr << "StopThread caught in JEtimator\n";
        throwInterruptException(env, "Interrupt from Estimator");
        return NULL;
    } 
    JNI_EXCEPTIONS;
    
    return NULL;
}
