#include <JNIUtil.h>
#include <base/XDEException.h>
#include <jni.h>
#include <objectfunction/XDEObjectiveFunction.h>
#include <optimizer/OptimizerFactory.h>

#include "edu_rochester_urmc_cbim_jni_JOptimizer.h"

static OptimizerFactory optimizerFactory;

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JOptimizer
 * Method:    initializeOptimizer
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_JOptimizer_initializeOptimizer(
    JNIEnv *env, jobject obj, jstring optimizerName) {
    try {
        const char *str = env->GetStringUTFChars(optimizerName, NULL);
        OptimizerPtr optimizer(optimizerFactory.createMethod(string(str)));
        assignTheNativeObject(env, obj, optimizer);
        env->ReleaseStringUTFChars(optimizerName, str);
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JOptimizer
 * Method:    s_getAvailableMethods
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JOptimizer_s_1getAvailableMethods(JNIEnv *env,
                                                                   jclass cls) {
    return getAvailableMethods(env, optimizerFactory);
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JOptimizer
 * Method:    isAttributeDefined
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_rochester_urmc_cbim_jni_JOptimizer_isAttributeDefined(
    JNIEnv *env, jclass cls, jstring method, jstring attribute) {
    return isAttributeDefined(env, optimizerFactory, method, attribute);
}

