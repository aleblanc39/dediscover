#include <edu_rochester_urmc_cbim_jni_JIntermediateSymbol.h>
#include <JNIUtil.h>
#include <parser/IntermediateSymbol.h>
using namespace std;


/*
 * Class:     edu_rochester_urmc_cbim_jni_JIntermediateSymbol
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JIntermediateSymbol_getName
 (JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr param = boost::any_cast<IntermediateSymbolPtr> (getNativePointer(env, obj));
	return env->NewStringUTF(param->getName().c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}
/*
 * Class:     edu_rochester_urmc_cbim_jni_JIntermediateSymbol
 * Method:    getDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JIntermediateSymbol_getDescription
  (JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr param = boost::any_cast<IntermediateSymbolPtr> (getNativePointer(env, obj));
	return env->NewStringUTF(param->getDescription().c_str());
    } JNI_EXCEPTIONS;
    return NULL;

}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JIntermediateSymbol
 * Method:    getFormula
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JIntermediateSymbol_getFormula
  (JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr param = boost::any_cast<IntermediateSymbolPtr> (getNativePointer(env, obj));
	return env->NewStringUTF(param->getExpressionString().c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}
/*
 * Class:     edu_rochester_urmc_cbim_jni_JIntermediateSymbol
 * Method:    getDisplayExpression
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JIntermediateSymbol_getDisplayExpression
  (JNIEnv *env, jobject obj) {
    try {
	IntermediateSymbolPtr param = boost::any_cast<IntermediateSymbolPtr> (getNativePointer(env, obj));
	return env->NewStringUTF(param->getDisplayExpression().c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}
/*
 * Class:     edu_rochester_urmc_cbim_jni_JIntermediateSymbol
 * Method:    setDescription
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_JIntermediateSymbol_setDescription
(JNIEnv *env, jobject obj, jstring desc) {
    try {
	IntermediateSymbolPtr intermediateSymbol = boost::any_cast<IntermediateSymbolPtr>(getNativePointer(env, obj));
	const char *str = env -> GetStringUTFChars(desc, NULL);
	intermediateSymbol->setDescription(string(str));
	env->ReleaseStringUTFChars(desc, str);
    } JNI_EXCEPTIONS;

}
