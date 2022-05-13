#include <edu_rochester_urmc_cbim_jni_JInitialCondition.h>
#include <parser/DependentVariable.h>
#include <JNIUtil.h>
#include <string>
#include <boost/pointer_cast.hpp>
#include <iostream>

using namespace std;



/*
 * Class:     edu_rochester_urmc_cbim_jni_JInitialCondition
 * Method:    hasFormula
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_edu_rochester_urmc_cbim_jni_JInitialCondition_hasFormula
(JNIEnv *env, jobject obj) {
    try {
 	ModelSymbolPtr param = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	DependentVariablePtr dv = boost::static_pointer_cast<DependentVariable>(param);

	return boost::numeric_cast<jboolean>( (dv -> getInitialConditionExpression()) != NULL);
    } JNI_EXCEPTIONS;
    return boost::numeric_cast<jboolean>(false);
}


/*
 * Class:     edu_rochester_urmc_cbim_jni_JInitialCondition
 * Method:    getFormula
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_edu_rochester_urmc_cbim_jni_JInitialCondition_getFormula
(JNIEnv *env, jobject obj) {
    try {
 	ModelSymbolPtr param = boost::any_cast<ModelSymbolPtr> (getNativePointer(env, obj));
	DependentVariablePtr dv = boost::static_pointer_cast<DependentVariable>(param);


	string s;
	if (dv->getInitialConditionExpression() != NULL)
	    s = dv->getInitialConditionExpression()->getOriginalExpression().c_str();
	return env->NewStringUTF(s.c_str());
    } JNI_EXCEPTIONS;
    return NULL;
}
