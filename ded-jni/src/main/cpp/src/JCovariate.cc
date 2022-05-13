#include <edu_rochester_urmc_cbim_jni_JCovariate.h>
#include <memory>
#include <JNIUtil.h>
#include <parser/Covariate.h>

/*
 * Class:     edu_rochester_urmc_cbim_jni_JCovariate
 * Method:    isMapped
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_rochester_urmc_cbim_jni_JCovariate_isMapped
(JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr is = boost::any_cast<IntermediateSymbolPtr>(getNativePointer(env, obj));
	CovariatePtr cov = std::dynamic_pointer_cast<Covariate>(is);
	return cov -> isMapped();
    } JNI_EXCEPTIONS;
    return false;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JCovariate
 * Method:    setTVFMapped
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JCovariate_setTVFMapped
(JNIEnv *env, jobject obj, jboolean b) {
    try {
	IntermediateSymbolPtr is = boost::any_cast<IntermediateSymbolPtr>(getNativePointer(env, obj));
	CovariatePtr cov = std::dynamic_pointer_cast<Covariate>(is);
	if (b)
	    cov -> mapFunction();
	else
	    cov -> unmapFunction();
    } JNI_EXCEPTIONS;
}



/*
 * Class:     edu_rochester_urmc_cbim_jni_JCovariate
 * Method:    getTimeVaryingFunction
 * Signature: ()Ledu/rochester/urmc/cbim/jni/JTimeVaryingFunction;
 */
JNIEXPORT jobject JNICALL Java_edu_rochester_urmc_cbim_jni_JCovariate_getTimeVaryingFunction
(JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr is = boost::any_cast<IntermediateSymbolPtr>(getNativePointer(env, obj));
	CovariatePtr cov = std::dynamic_pointer_cast<Covariate>(is);

	jobject newobj = createDefaultJavaObject(env,
						 "edu/rochester/urmc/cbim/jni/JTimeVaryingFunction",
						 cov -> getTVFunction());
	return newobj;
    } JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JCovariate
 * Method:    setTimeVaryingFunction
 * Signature: (Ledu/rochester/urmc/cbim/jni/JTimeVaryingFunction;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JCovariate_setTimeVaryingFunction
(JNIEnv *env, jobject obj, jobject timeVaryingFunction) {

    try {
	IntermediateSymbolPtr is = boost::any_cast<IntermediateSymbolPtr>(getNativePointer(env, obj));
	CovariatePtr cov = std::dynamic_pointer_cast<Covariate>(is);

	TimeVaryingFunctionPtr function;
	if (timeVaryingFunction != NULL) {
	    function = boost::any_cast<TimeVaryingFunctionPtr>(getNativePointer(env, timeVaryingFunction));
	    function -> setContainsVariableControls(false); // Why??
	    cov -> assignTVFunction(function);
	}
    } JNI_EXCEPTIONS;
}
