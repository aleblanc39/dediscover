#include <edu_rochester_urmc_cbim_jni_SolverResult.h>
#include <solver/SolverResult.h>
#include "JNIUtil.h"

using namespace std;
/*
 * Class:     edu_rochester_urmc_cbim_jni_SolverResult
 * Method:    getTimepoints
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_rochester_urmc_cbim_jni_SolverResult_getTimepoints
(JNIEnv *env, jobject obj) {
    try {
	SolverResultPtr res = boost::any_cast<SolverResultPtr> (getNativePointer(env, obj));
	TDoubleVector vec = res->getTimepoints();
	return createJavaVector(env, vec);
    } JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_SolverResult
 * Method:    getResults
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_SolverResult_getResults
(JNIEnv *env, jobject obj) {
    try {
	SolverResultPtr res = boost::any_cast<SolverResultPtr> (getNativePointer(env, obj));
	const TDoubleMatrix &matrix = res->getData();
	return createJavaDoubleMatrix(env, matrix);
    } JNI_EXCEPTIONS;
    return NULL;

}

/*
 * Class:     edu_rochester_urmc_cbim_jni_SolverResult
 * Method:    getVariableNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_SolverResult_getVariableNames
(JNIEnv *env, jobject obj) {
    try {
	SolverResultPtr res = boost::any_cast<SolverResultPtr> (getNativePointer(env, obj));
 	vector<string> vec = res->getVariableNames();
 	return createJavaVector(env, vec);
    } JNI_EXCEPTIONS;
    return NULL;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_SolverResult
 * Method:    getNbVariables
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_SolverResult_getNbVariables
(JNIEnv *env, jobject obj) {
    try {
	SolverResultPtr res = boost::any_cast<SolverResultPtr> (getNativePointer(env, obj));
	assert(res.get() != NULL);
	return boost::numeric_cast<jint>(res->getNbVariables());
    } JNI_EXCEPTIONS;
    return 9;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_SolverResult
 * Method:    getNbTimepoints
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_SolverResult_getNbTimepoints
(JNIEnv *env, jobject obj) {
    try {
	SolverResultPtr res = boost::any_cast<SolverResultPtr> (getNativePointer(env, obj));
	assert(res.get() != NULL);
	return boost::numeric_cast<jint>(res->getNbTimepoints());
    } JNI_EXCEPTIONS;
    return 0;
}
