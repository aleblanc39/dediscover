#include "edu_rochester_urmc_cbim_jni_Algorithm.h"

#include "JNIUtil.h"
#include <algorithms/XDEAlgorithm.h>
#include <algorithms/AlgorithmTemplates.h>

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_Algorithm
 * Method:    getMethodName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_Algorithm_getMethodName(
	JNIEnv *env, jobject obj) {
    try {
	XDEAlgorithm *algm = (XDEAlgorithm *)(getAlgorithmPtr(env, obj));
	string ret = algm->getMethodName();
	return env->NewStringUTF(ret.c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_Algorithm
 * Method:    getShortMethodDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_Algorithm_getShortMethodDescription(
JNIEnv *env, jobject obj) {
    try {
	XDEAlgorithm *algm = (XDEAlgorithm *)(getAlgorithmPtr(env, obj));
	string ret = algm->getShortMethodDescription();
	return env->NewStringUTF(ret.c_str());
    } JNI_EXCEPTIONS;
    return NULL;

}

/*
 * Class:     edu_rochester_urmc_cbim_jni_Algorithm
 * Method:    getLongMethodDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_Algorithm_getLongMethodDescription(
JNIEnv *env, jobject obj) {
    try {
	XDEAlgorithm *algm = (XDEAlgorithm *)(getAlgorithmPtr(env, obj));
	string ret = algm->getLongMethodDescription();
	return env->NewStringUTF(ret.c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_Algorithm
 * Method:    getMethodAttributes
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_Algorithm_getMethodAttributes(
JNIEnv *env, jobject obj) {
    try {
	XDEAlgorithm *algm = (XDEAlgorithm *)(getAlgorithmPtr(env, obj));
	vector<string> ret = algm->getMethodAttributes();
	return createJavaVector(env, ret);
    } JNI_EXCEPTIONS;
    return NULL;
}



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



