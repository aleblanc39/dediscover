#include "edu_rochester_urmc_cbim_jni_JXDECSVFile.h"
#include "JNIUtil.h"
#include <base/XDECSVFile.h>

using namespace std;
/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    setTimepoints
 * Signature: ([D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_setTimepoints
(JNIEnv *env, jobject obj, jdoubleArray timepoints) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	auto vec = createDoubleVector(env, timepoints);
	ptr -> setTimepoints(vec);
    } JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    setData
 * Signature: ([[D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_setData
(JNIEnv *env, jobject obj, jobjectArray data) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	TDoubleMatrix *mat = createDoubleMatrix(env, data);
	ptr -> setData(*mat);
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    setVariableNames
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_setVariableNames
(JNIEnv *env, jobject obj, jobjectArray varNames) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	vector<string>  vec = createStringVector(env, varNames);
	ptr -> setVariableNames(vec);
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    getTimepoints
 * Signature: ()[D
 */
JNIEXPORT jdoubleArray JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_getTimepoints
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	return createJavaVector(env, ptr->getTimepoints());
    } JNI_EXCEPTIONS;
    return NULL;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    getData
 * Signature: ()[[D
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_getData
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	return createJavaDoubleMatrix(env, ptr->getData());
    } JNI_EXCEPTIONS;
    return NULL;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    getVariableNames
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_getVariableNames
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	return createJavaVector(env, ptr->getVariableNames());
    } JNI_EXCEPTIONS;
    return NULL;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    getNbTimepoints
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_getNbTimepoints
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	return boost::numeric_cast<jint>(ptr->getNbTimepoints());
    } JNI_EXCEPTIONS;
    return 0;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    getNbVariables
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_getNbVariables
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	return boost::numeric_cast<jint>(ptr->getNbVariables());
    } JNI_EXCEPTIONS;
    return 0;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    createNativeObject
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_createNativeObject
(JNIEnv *env, jobject obj) {
    try {
	XDECSVFilePtr csvfile(new XDECSVFile());
	assignTheNativeObject(env, obj, csvfile);
    } JNI_EXCEPTIONS;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    addData
 * Signature: ([Ljava/lang/String;[D[[D)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_addData___3Ljava_lang_String_2_3D_3_3D
(JNIEnv *env, jobject obj, jobjectArray _varNames, jdoubleArray _timepoints, jobjectArray _data) {
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	assert(ptr.get() != NULL);
	auto timepoints = createDoubleVector(env, _timepoints);
	vector<string> variableNames = createStringVector(env, _varNames);
	TDoubleMatrix *mat = createDoubleMatrix(env, _data);

	ptr -> addData(variableNames, timepoints, *mat);
	delete mat;
    } JNI_EXCEPTIONS;

}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JXDECSVFile
 * Method:    addData
 * Signature: (Ledu/rochester/urmc/cbim/jni/JXDECSVFile;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JXDECSVFile_addData__Ledu_rochester_urmc_cbim_jni_JXDECSVFile_2
(JNIEnv *env, jobject obj, jobject otherData) {
    if (otherData == NULL)
	return;
    try {
	XDECSVFilePtr ptr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, obj));
	XDECSVFilePtr otherDataPtr = boost::any_cast<XDECSVFilePtr>(getNativePointer(env, otherData));
	ptr -> addData(otherDataPtr.get());
    } JNI_EXCEPTIONS;

}
