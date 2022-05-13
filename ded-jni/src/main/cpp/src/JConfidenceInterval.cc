
#include <jni.h>
#include <objectfunction/XDEObjectiveFunction.h>
#include <statistics/CIFactory.h>
#include <statistics/ConfidenceIntervalResult.h>
#include <statistics/NLSSummary.h>
#include <iostream>
#include "JNIUtil.h"
#include "edu_rochester_urmc_cbim_jni_JConfidenceInterval.h"

static CIFactory ciFactory;

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceInterval
 * Method:    initializeMethod
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceInterval_initializeMethod(
    JNIEnv *env, jobject obj, jstring jname) {
    try {
        const char *str = env->GetStringUTFChars(jname, NULL);
        ConfidenceIntervalPtr ci(ciFactory.createMethod(string(str)));
        assignTheNativeObject(env, obj, ci);
        env->ReleaseStringUTFChars(jname, str);
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceInterval
 * Method:    nat_evaluate
 * Signature: (Ledu/rochester/urmc/cbim/jni/EstimatorResult;Ledu/rochester/urmc/cbim/jni/JDDEModel;Ledu/rochester/urmc/cbim/jni/JSolver;Ledu/rochester/urmc/cbim/jni/JOptimizer;[D[Ljava/lang/String;[[D)Ledu/rochester/urmc/cbim/jni/EstimatorResult;
 */
JNIEXPORT jobject JNICALL Java_edu_rochester_urmc_cbim_jni_JConfidenceInterval_nat_1evaluate
  (JNIEnv *env, jobject obj, jobjectArray jparamValues, jobject jestimatorResult, jobject jmodel, jobject jsolver, jobject joptimizer, jdoubleArray jtimepoints, jobjectArray jsymbolNames, 
  jobjectArray jdata) {
      try {
         ConfidenceIntervalPtr ci =
            boost::any_cast<ConfidenceIntervalPtr>(getNativePointer(env, obj));

    
        ModelPtr model = boost::any_cast<ModelPtr>(getNativePointer(env, jmodel));
        EstimatorResultPtr estimatorResult = boost::any_cast<EstimatorResultPtr>(getNativePointer(env, jestimatorResult));
        SolverPtr solver = boost::any_cast<SolverPtr>(getNativePointer(env, jsolver));
        OptimizerPtr optimizer = boost::any_cast<OptimizerPtr>(getNativePointer(env, joptimizer));
        vector<string> formulas = createStringVector(env, jsymbolNames);
        TDoubleMatrix *columnData = createDoubleMatrix(env, jdata);
        vector<double> timepointsUsed = createDoubleVector(env, jtimepoints);
        DataTableMappingPtr mapping(new DataTableMapping(timepointsUsed, formulas, *columnData));

        EstimationParameterSetPtr parameterSet =
            createEstimationParameterSet(env, jparamValues);


        ConfidenceIntervalResultPtr ciResult = ci->evaluate(
            estimatorResult -> getOptimizationResults(),
            mapping,
            parameterSet,
            model,
            solver
        );

        // TODO Probably don't want to return the default.
        jobject jCIResult = createDefaultJavaObject(
            env, "edu/rochester/urmc/cbim/jni/JConfidenceIntervalResult", ciResult);

        return jCIResult;

      } JNI_EXCEPTIONS;
      return NULL;

  }






/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceInterval
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceInterval_s_1getAvailableMethods(
    JNIEnv *env, jclass cls) {
    return getAvailableMethods(env, ciFactory);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceInterval
 * Method:    getNLSSummaryName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceInterval_getNLSSummaryName(
    JNIEnv *env, jclass cls) {
    try {
        std::string s = NLSSummary::s_getMethodName();
        return env->NewStringUTF(s.c_str());
    }
    JNI_EXCEPTIONS;
    return NULL;
}
