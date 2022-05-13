#include <edu_rochester_urmc_cbim_jni_JModelSymbol.h>
#include <parser/ModelSymbol.h>
#include <JNIUtil.h>
#include <string>
#include <iostream>

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_JModelSymbol
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JModelSymbol__1getName
(JNIEnv *env, jobject obj) {
    try {
	ModelSymbolPtr param = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	return env->NewStringUTF(param->getName().c_str());
    } JNI_EXCEPTIONS;
    return NULL;

}




/*
 * Class:     edu_rochester_urmc_cbim_jni_JModelSymbol
 * Method:    getFirstOccurenceLine
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_JModelSymbol_getFirstOccurenceLine
(JNIEnv *env, jobject obj) {
    try {
	ModelSymbolPtr ms = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	return ms -> getFirstOccurenceLine();
    } JNI_EXCEPTIONS;
    return 0;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JModelSymbol
 * Method:    getDefinitionLine
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_JModelSymbol_getDefinitionLine
(JNIEnv *env, jobject obj) {
    try {
	ModelSymbolPtr ms = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	return ms -> getDefinitionLine();
    } JNI_EXCEPTIONS;
    return 0;
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JModelSymbol
 * Method:    getOccurenceOrder
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_edu_rochester_urmc_cbim_jni_JModelSymbol_getOccurenceOrder
(JNIEnv *env, jobject obj) {
    try {
	ModelSymbolPtr ms = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	assert(ms.get() != NULL);
	return ms -> getOccurenceIndex();
    } JNI_EXCEPTIONS;
    return 0;
}
