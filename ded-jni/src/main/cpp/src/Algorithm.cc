#include "edu_rochester_urmc_cbim_jni_Algorithm.h"

#include "JNIUtil.h"
#include <algorithms/XDEAlgorithm.h>
#include <algorithms/AlgorithmTemplates.h>

using namespace std;



/*
 * Class:     edu_rochester_urmc_cbim_jni_Algorithm
 * Method:    getControlParameters
 * Signature: ()[Ledu/rochester/urmc/cbim/jni/JGeneralParameter;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_Algorithm_getControlParameters
(JNIEnv *env, jobject obj) {
    XDEAlgorithm *algm = (XDEAlgorithm *)(getAlgorithmPtr(env, obj));
    return getParameters(env, algm);
}



