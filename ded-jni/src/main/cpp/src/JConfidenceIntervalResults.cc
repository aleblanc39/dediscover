/*
 * JConfidenceIntervalResults.cc
 *
 */

#include <statistics/ConfidenceIntervalResult.h>

#include "JNIUtil.h"
#include "edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult.h"
using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getParameterValues
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getParameterValues(
    JNIEnv *env, jobject obj) {
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getParameterValues();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getStandardErrors
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getStandardErrors(
    JNIEnv *env, jobject obj) {
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getStandardError();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getLowerBounds
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getLowerBounds(
    JNIEnv *env, jobject obj) {
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getLowerBounds();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getUpperBounds
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getUpperBounds(
    JNIEnv *env, jobject obj) {
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getUpperBounds();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getTStat
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getTStat(
    JNIEnv *env, jobject obj) {
      std::cerr << "Getting tstat values.\n";

    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getTstat();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getPValues
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getPValues(
    JNIEnv *env, jobject obj) {
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        auto vals = res->getPvalues();
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult
 * Method:    getParameterNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JConfidenceIntervalResult_getParameterNames(
    JNIEnv *env, jobject obj) {
      std::cerr << "Getting the parameter names\n";
    try {
        ConfidenceIntervalResultPtr res =
            boost::any_cast<ConfidenceIntervalResultPtr>(
                getNativePointer(env, obj));
        vector<string> vals = res->getParameterNames();
        std::cerr << "Nb parameters: " << vals.size() << std::endl;
        return createJavaVector(env, vals);
    }
    JNI_EXCEPTIONS;
    return NULL;
}
