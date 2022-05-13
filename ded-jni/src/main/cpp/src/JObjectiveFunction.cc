#include "edu_rochester_urmc_cbim_jni_JObjectiveFunction.h"
#include "JNIUtil.h"
#include <objectfunction/XDEObjectiveFunction.h>
#include <objectfunction/ObjectiveFunctionFactory.h>

static ObjectiveFunctionFactory objectiveFunctionFactory;


/*
 * Class:     edu_rochester_urmc_cbim_jni_JObjectiveFunction
 * Method:    initializeObject
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JObjectiveFunction_initializeObject
(JNIEnv *env, jobject obj, jstring name) {
    try {
	const char *str = env -> GetStringUTFChars(name, NULL);
	ObjectiveFunctionPtr objectiveFunction(objectiveFunctionFactory.createMethod(std::string(str)));
	assignTheNativeObject(env, obj, objectiveFunction);
	env->ReleaseStringUTFChars(name, str);
    } JNI_EXCEPTIONS;
}



/*
 * Class:     edu_rochester_urmc_cbim_jni_JObjectiveFunction
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_JObjectiveFunction_s_1getAvailableMethods
(JNIEnv *env, jclass cls) {
    return getAvailableMethods(env, objectiveFunctionFactory);
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JObjectiveFunction
 * Method:    isAttributeDefined
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_edu_rochester_urmc_cbim_jni_JObjectiveFunction_isAttributeDefined
(JNIEnv *env, jclass cls, jstring method, jstring attribute) {
    return isAttributeDefined(env, objectiveFunctionFactory, method, attribute);
}
