#include <JNIUtil.h>
#include <base/XDEException.h>
#include <jni.h>
#include <statistics/EstimatorResult.h>

#include "edu_rochester_urmc_cbim_jni_EstimatorResult.h"

using namespace std;
/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getNumberOfEvaluations
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getNumberOfEvaluations(
    JNIEnv *env, jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (int)res->getNumberOfEvaluations();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getOptimalValue
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getOptimalValue(JNIEnv *env,
                                                                 jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (double)res->getOptimalValue();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getRSS
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getRSS(JNIEnv *env,
                                                        jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (double)res->getRSS();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getAIC
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getAIC(JNIEnv *env,
                                                        jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (double)res->getAIC();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getAICc
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getAICc(JNIEnv *env,
                                                         jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (double)res->getAICc();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getBIC
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getBIC(JNIEnv *env,
                                                        jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (double)res->getBIC();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getNParam
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getNParam(JNIEnv *env,
                                                           jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (int)res->getNParam();
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getNObs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getNObs(
    JNIEnv *env, jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        return (int)res->getNObs();
    }
    JNI_EXCEPTIONS;
    return 0;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getBestResult
 * Signature: ()[[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getBestResult(JNIEnv *env,
                                                               jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        const auto  &vec = res->getBestResult();
        return createJavaVector(env, vec);
    }
    JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_EstimatorResult
 * Method:    getEstimatedParameterNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_EstimatorResult_getEstimatedParameterNames(
    JNIEnv *env, jobject obj) {
    try {
        EstimatorResultPtr res =
            boost::any_cast<EstimatorResultPtr>(getNativePointer(env, obj));
        vector<string> vec = res->getEstimatedParameterNames();
        return createJavaVector(env, vec);
    }
    JNI_EXCEPTIONS;
    return 0;
}

