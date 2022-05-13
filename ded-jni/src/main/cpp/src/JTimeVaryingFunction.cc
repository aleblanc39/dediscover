#include "edu_rochester_urmc_cbim_jni_JTimeVaryingFunction.h"

#include <JNIUtil.h>

#include <extra_parser_functions/TimeVaryingFunctionFactory.h>

#include <boost/cast.hpp>
#include <iostream>
using namespace std;

static TimeVaryingFunctionFactory timeVaryingFunctionFactory;


static void createDumyFunction(JNIEnv *env, jobject obj);
/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    createTimeVaryingFunction
 * Signature: (Ljava/lang/String;)V
 */

JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_createTimeVaryingFunction
(JNIEnv *env, jobject obj, jstring functionName) {

    /**
     * If no function name is specified take the first one.
     */
    try {

	if (functionName == NULL) {
	    createDumyFunction(env, obj);
	} else {
	    const char *str = env -> GetStringUTFChars(functionName, NULL);
	    TimeVaryingFunctionPtr newFunction (timeVaryingFunctionFactory.createMethod(string(str)));
	    
	    assignTheNativeObject(env, obj, newFunction);
	    env -> ReleaseStringUTFChars(functionName, str);
	}
    } JNI_EXCEPTIONS;
}



/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    nat_setControlPoints
 * Signature: ([[D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_nat_1setControlPoints
(JNIEnv *env, jobject obj, jobjectArray jpoints) {
    try {
	TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr> (getNativePointer(env, obj));
	TDoubleMatrix *points = createDoubleMatrix(env, jpoints);
	unsigned nbPoints = points -> size1();
	tvf -> reset();
	for (unsigned i = 0; i < nbPoints; i++) {
	    tvf -> addControlPoint((*points)(i, 0), (*points)(i,1));
	}
	tvf -> mapVariables();
    } JNI_EXCEPTIONS;

}





/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    compute
 * Signature: (D)D
 */
JNIEXPORT jdouble JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_compute
(JNIEnv *env, jobject obj, jdouble value) {
    try {
	TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr> (getNativePointer(env, obj));
	double ret = tvf->compute(boost::numeric_cast<double>(value));
	return boost::numeric_cast<jdouble>(ret);
    } JNI_EXCEPTIONS;
    return 0;
}



void createDumyFunction(JNIEnv *env, jobject obj) {
    try {
	TimeVaryingFunctionPtr newFunction 
	    (timeVaryingFunctionFactory.createMethod(timeVaryingFunctionFactory.getKnownMethods()[0]));
	assignTheNativeObject(env, obj, newFunction);
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    getUsedTimepoints
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_getUsedTimepoints
(JNIEnv *env, jobject obj) {
    try {
	TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr> (getNativePointer(env, obj));
	TDoubleVector ret = tvf->getUsedTimepoints();
	return createJavaVector(env, ret);
    } JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    reset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_reset
(JNIEnv *env, jobject obj){ 
    try {
	std::cerr << "TimeVaryingFunction::reset called\n";
	TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr>
	    (getNativePointer(env, obj));
	tvf -> reset();
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    nat_addControlPoints
 * Signature: ([[D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_nat_1addControlPoints
(JNIEnv *env, jobject obj, jobjectArray jpoints) {
    try {

	// TODO Same code as setControlPoints... factor out
	TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr> (getNativePointer(env, obj));
	TDoubleMatrix *points = createDoubleMatrix(env, jpoints);
	unsigned nbPoints = points -> size1();
	for (unsigned i = 0; i < nbPoints; i++) {
	    tvf -> addControlPoint((*points)(i, 0), (*points)(i,1));
	}
	tvf -> mapVariables();
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    nat_initialize
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_nat_1initialize
  (JNIEnv *env, jobject jobj) {

  try {
    TimeVaryingFunctionPtr tvf = boost::any_cast<TimeVaryingFunctionPtr> (getNativePointer(env, jobj));
    tvf -> initialize();
  } JNI_EXCEPTIONS;



  }



/*
 * Class:     edu_rochester_urmc_cbim_jni_JTimeVaryingFunction
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_JTimeVaryingFunction_s_1getAvailableMethods

(JNIEnv *env, jclass cls) {
    return getAvailableMethods(env, timeVaryingFunctionFactory);
}
