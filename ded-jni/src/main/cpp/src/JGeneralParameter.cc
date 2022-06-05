#include <JNIUtil.h>
#include <algorithms/ConcreteParameters.h>
#include <base/XDEException.h>
#include <edu_rochester_urmc_cbim_jni_JGeneralParameter.h>

#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <typeinfo>

/**
 * TODO Class is working so far because there is no list of parameter options
 * that are double. So far only have boolean, strings. Not working for ints
 */

using namespace std;

typedef enum { MAX_FCN, MIN_FCN, VALUE_FCN, DEFAULT_FCN } functionCall;

static jobject getObject(JNIEnv *env, GeneralParameterPtr param,
                         functionCall method);
static jobject getObject(JNIEnv *env, GeneralParameterPtr param,
                         string strValue);

static jobjectArray createObjectArray(JNIEnv *env, GeneralParameterPtr param,
                                      const vector<string> &strValue);

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    getName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getName(JNIEnv *env,
                                                           jobject obj) {
    try {
        GeneralParameterPtr param =
        boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return env->NewStringUTF(param->getName().c_str());
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    getDescription
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getDescription(JNIEnv *env,
                                                                  jobject obj) {
    try {
        // GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
        // obj);
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return env->NewStringUTF(param->getDescription().c_str());
    }
    JNI_EXCEPTIONS;
    return NULL;
}

JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getValue(JNIEnv *env,
                                                            jobject obj) {
    try {
        // GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
        // obj);
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        jobject ret = getObject(env, param, VALUE_FCN);
        return ret;
    }
    JNI_EXCEPTIONS;
    return NULL;
}

JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getMin(JNIEnv *env,
                                                          jobject obj) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return getObject(env, param, MIN_FCN);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getMax(JNIEnv *env,
                                                          jobject obj) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        assert(param.get() != NULL);
        return getObject(env, param, MAX_FCN);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    hasMin
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_hasMin(JNIEnv *env,
                                                          jobject obj) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        assert(param.get() != NULL);
        return param->hasMinValue();
    }
    JNI_EXCEPTIONS;
    return false;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    hasMax
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_hasMax(JNIEnv *env,
                                                          jobject obj) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        assert(param.get() != NULL);
        return param->hasMaxValue();
    }
    JNI_EXCEPTIONS;
    return false;
}

JNIEXPORT jobject JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getDefault(JNIEnv *env,
                                                              jobject obj) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return getObject(env, param, DEFAULT_FCN);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    setDescription
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_setDescription(
    JNIEnv *env, jobject obj, jstring desc) {
    // GeneralParameter *param = (GeneralParameter *) getBasePointer(env, obj);
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        const char *str = env->GetStringUTFChars(desc, NULL);
        param->setDescription(string(str));
        env->ReleaseStringUTFChars(desc, str);
    }
    JNI_EXCEPTIONS;
}

template <class GP>
GenericParameter<GP> *getConcreteObject(GeneralParameter *param) {
    return (GenericParameter<GP> *)param;
}

template <class GP>
string getStringMin(GeneralParameter *param) {
    return boost::lexical_cast<string>((GenericParameter<GP> *)param)->getMin();
}

jobject getObject(JNIEnv *env, GeneralParameterPtr param, functionCall method) {
    jobject newobj;

    if (typeid(*param.get()) == typeid(IntParameter)) {
        // Declaring the following two static caused a SIGSEV.
        jclass intClass = env->FindClass("java/lang/Integer");
        jmethodID intID = env->GetMethodID(intClass, "<init>", "(I)V");
        int value = 0;
        IntParameter *intParam =
            boost::polymorphic_cast<IntParameter *>(param.get());
        switch (method) {
            case VALUE_FCN:
                value = intParam->getValue();
                break;
            case MIN_FCN:
                value = intParam->getMin();
                break;
            case MAX_FCN:
                value = intParam->getMax();
                break;
            case DEFAULT_FCN:
                value = intParam->getDefault();
                break;
            default:
                throwXDEException(env, "Unknown operation for integer type");
        }

        newobj = env->NewObject(intClass, intID, value);
    } else if (typeid(*param) == typeid(DoubleParameter)) {
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleID = env->GetMethodID(doubleClass, "<init>", "(D)V");
        double value = 0;
        DoubleParameter *dblParam =
            boost::polymorphic_cast<DoubleParameter *>(param.get());

        switch (method) {
            case VALUE_FCN:
                value = dblParam->getValue();
                break;
            case MIN_FCN:
                value = dblParam->getMin();
                break;
            case MAX_FCN:
                value = dblParam->getMax();
                break;
            case DEFAULT_FCN:
                value = dblParam->getDefault();
                break;
            default:
                throwXDEException(env, "Unknown operation for double type");
                return NULL;
        }
        newobj = env->NewObject(doubleClass, doubleID, value);
    } else if (typeid(*param) == typeid(BoolParameter)) {
        // TODO: Java docs recommend against creating new Boolean objects.
        // Figure out
        // why CallStaticObjetMethod doesn't work.
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        // jmethodID booleanID = env ->GetStaticMethodID(booleanClass,
        // "valueOf", "(Z)Ljava/lang/Boolean");
        jmethodID booleanID = env->GetMethodID(booleanClass, "<init>", "(Z)V");

        jboolean value;
        BoolParameter *boolParam =
            boost::polymorphic_cast<BoolParameter *>(param.get());
        switch (method) {
            case VALUE_FCN:
                value = boost::numeric_cast<jboolean>(boolParam->getValue());
                break;
            case DEFAULT_FCN:
                value = boolParam->getDefault();
                break;
            default:
                throwXDEException(env, "Unknown operation for boolean type");
                return NULL;
        }
        newobj = env->NewObject(booleanClass, booleanID, value);
    } else if (typeid(*param) == typeid(StringParameter)) {
        // TODO: Check for memory allocation?
        StringParameter *strParam =
            boost::polymorphic_cast<StringParameter *>(param.get());
        switch (method) {
            case VALUE_FCN:
                newobj = env->NewStringUTF((strParam->getValue()).c_str());
                break;
            case DEFAULT_FCN:
                newobj = env->NewStringUTF((strParam->getDefault()).c_str());
                break;
            default:
                throw XDEException("Unknown operation for string type");
        }
    } else {
        throwSystemException(env, string("Unknown type for general parameter: ")
                                      .append(typeid(*param).name()));
    }

    return newobj;
}

/**
 * Create a java object  of type "type" with the value defined by the
 * string 'value'
 */
jobject getObject(JNIEnv *env, GeneralParameterPtr param, string strValue) {
    jobject newobj;

    if (typeid(*param.get()) == typeid(IntParameter)) {
        // Declaring the following two static caused a SIGSEV.
        jclass intClass = env->FindClass("java/lang/Integer");
        jmethodID intID = env->GetMethodID(intClass, "<init>", "(I)V");
        int value = atoi(strValue.c_str());
        newobj = env->NewObject(intClass, intID, value);
    } else if (typeid(*param) == typeid(DoubleParameter)) {
        assert(0);
        std::cerr << "Went through assert(0). Why?\n";
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleID = env->GetMethodID(doubleClass, "<init>", "(D)V");
        double value;  // XXX = getDouble(strValue);
        newobj = env->NewObject(doubleClass, doubleID, value);
        assert(0);
    } else if (typeid(*param) == typeid(BoolParameter)) {
        // TODO: Java docs recommend against creating new Boolean objects.
        // Figure out
        // why CallStaticObjetMethod doesn't work.
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        jmethodID booleanID = env->GetMethodID(booleanClass, "<init>", "(Z)V");
        jboolean value = true;
        char c = strValue.at(0);
        if (c == '0' || c == 'f' || c == 'F') value = false;
        newobj = env->NewObject(booleanClass, booleanID, value);
    } else if (typeid(*param) == typeid(StringParameter)) {
        // TODO: Check for memory allocation?
        newobj = env->NewStringUTF(strValue.c_str());
    } else {
        throwXDEException(env, string("Unknown type for general parameter: "));
        return NULL;
    }
    return newobj;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    setValue
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_setValue(JNIEnv *env,
                                                            jobject obj,
                                                            jobject value) {
    try {
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        jclass objClass = env->FindClass("java/lang/Object");
        jmethodID objID =
            env->GetMethodID(objClass, "toString", "()Ljava/lang/String;");
        jstring strVal = (jstring)(env->CallObjectMethod(value, objID));
        const char *str = env->GetStringUTFChars(strVal, NULL);
        param->setStringValue(str);
        env->ReleaseStringUTFChars(strVal, str);

    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    hasOptions
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_hasOptions(JNIEnv *env,
                                                              jobject obj) {
    try {
        // GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
        // obj);
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return param->hasOptions();
    }
    JNI_EXCEPTIONS;
    return false;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    nbOptionsRequired
 * Signature: ()I
 */
JNIEXPORT jint JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_nbOptionsRequired(
    JNIEnv *env, jobject obj) {
    try {
        // GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
        // obj);
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        return param->getNbRequiredOptions();
    }
    JNI_EXCEPTIONS;
    return 0;
}

// /*
//  * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
//  * Method:    setValues
//  * Signature: ([Ljava/lang/Object;)Ljava/lang/Object;
//  *
//  * Setting new values. Will convert everything to strings regardless
//  * of original type.
//  */
// JNIEXPORT void JNICALL
// Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_setValues (JNIEnv *env,
// jobject obj, jobjectArray newValues) {
//     try {
// 	//GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
// obj); 	GeneralParameterPtr param =
// boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));

// 	jclass objClass = env->FindClass("java/lang/Object");
// 	jmethodID objID = env ->GetMethodID(objClass, "toString",
// "()Ljava/lang/String;");

// 	int size = env -> GetArrayLength(newValues);
// 	vector<string> valuesToSet(size);
// 	for (int i = 0; i < size; i++) {
// 	    jobject nextObj = env -> GetObjectArrayElement(newValues, i);
// 	    jstring strVal = (jstring) (env->CallObjectMethod(nextObj, objID));
// 	    const char *str = env -> GetStringUTFChars(strVal, NULL);
// 	    valuesToSet[i] = string(str);
// 	    env->ReleaseStringUTFChars(strVal, str);
// 	}

// 	param->setStringValues(valuesToSet);
//     } JNI_EXCEPTIONS;
// }

/*
 * Class:     edu_rochester_urmc_cbim_jni_JGeneralParameter
 * Method:    getOptions
 * Signature: ()[Ljava/lang/Object;
 */
JNIEXPORT jobjectArray JNICALL
Java_edu_rochester_urmc_cbim_jni_JGeneralParameter_getOptions(JNIEnv *env,
                                                              jobject obj) {
    try {
        // GeneralParameter *param = (GeneralParameter *) getBasePointer(env,
        // obj);
        GeneralParameterPtr param =
            boost::any_cast<GeneralParameterPtr>(getNativePointer(env, obj));
        vector<string> strValues = param->getStringOptions();
        return createObjectArray(env, param, strValues);
    }
    JNI_EXCEPTIONS;
    return NULL;
}

/**
 * Create an array of java objects of type "type" from the vector of strings.
 */

jobjectArray createObjectArray(JNIEnv *env, GeneralParameterPtr param,
                               const vector<string> &strValues) {
    jclass classType;

    if (typeid(*param.get()) == typeid(IntParameter)) {
        classType = env->FindClass("java/lang/Integer");
    } else if (typeid(*param.get()) == typeid(DoubleParameter)) {
        classType = env->FindClass("java/lang/Double");
    } else if (typeid(*param.get()) == typeid(BoolParameter)) {
        classType = env->FindClass("java/lang/Boolean");
    } else if (typeid(*param.get()) == typeid(StringParameter)) {
        return createJavaVector(env, strValues);
    } else {
        throw XDEException(string("Unknown type for general parameter: "));
    }
    assert(classType != NULL);

    jobjectArray ret = env->NewObjectArray(strValues.size(), classType, NULL);
    for (unsigned i = 0; i < strValues.size(); i++) {
        env->SetObjectArrayElement(ret, i, getObject(env, param, strValues[i]));
    }
    return ret;
}
