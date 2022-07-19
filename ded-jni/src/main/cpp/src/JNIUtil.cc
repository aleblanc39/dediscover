#include <JNIUtil.h>
#include <base/XDEMessage.h>
#include <base/XDEUtil.h>

#include <cassert>
using namespace std;



jlong getAlgorithmPtr(JNIEnv *env, jobject obj) {
    jclass cls = env->GetObjectClass(obj);
    assert(cls != NULL);

    jfieldID algmPtr = env->GetFieldID(cls, "algorithmPtr", "J");
    assert(algmPtr != NULL);
    return env->GetLongField(obj, algmPtr);
}

jlong getThreadPtr(JNIEnv *env, jobject obj) {
    jclass cls = env->GetObjectClass(obj);
    assert(cls != NULL);

    jfieldID threadPtr = env->GetFieldID(cls, "threadPtr", "J");
    assert(threadPtr != NULL);
    return env->GetLongField(obj, threadPtr);
}

jlong getObjectIndex(JNIEnv *env, jobject obj) {
    jclass cls = env->GetObjectClass(obj);
    assert(cls != NULL);

    jfieldID threadPtr = env->GetFieldID(cls, "nativeObjectIndex", "J");
    assert(threadPtr != NULL);
    return env->GetLongField(obj, threadPtr);
}



/**
 * Code to convert C++ exception to Java exception.
 */

void throwSystemException(JNIEnv *env) {
    jclass exception = env->FindClass(
        "edu/rochester/urmc/cbim/jni/exception/XDESystemException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, "An undefined exception has occured.");
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwSystemException(JNIEnv *env, string s) {
    jclass exception = env->FindClass(
        "edu/rochester/urmc/cbim/jni/exception/XDESystemException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, s.c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwXDEException(JNIEnv *env, const XDEException &e) {
    jclass exception =
        env->FindClass("edu/rochester/urmc/cbim/jni/exception/XDEException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, e.getMsg().c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwXDEException(JNIEnv *env, string s) {
    jclass exception =
        env->FindClass("edu/rochester/urmc/cbim/jni/exception/XDEException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, s.c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwInterruptException(JNIEnv *env, string s) {
    jclass exception = env->FindClass(
        "edu/rochester/urmc/cbim/jni/exception/InterruptException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, s.c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwParserException(JNIEnv *env, string s) {
    jclass exception =
        env->FindClass("edu/rochester/urmc/cbim/jni/exception/ParserException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, s.c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwParameterRangeException(JNIEnv *env, ParameterRangeException *e) {
    jclass exception = env->FindClass(
        "edu/rochester/urmc/cbim/jni/exception/ParameterRangeException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, "Parameter range exception");
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

void throwOutOfMemoryException(JNIEnv *env, string s) {
    jclass exception = env->FindClass(
        "edu/rochester/urmc/cbim/jni/exception/OutOfMemoryException");
    if (exception == NULL) {
        XDEMessage::log(XDEMessage::WARNING,
                        "Could not create exception object.");
        // Should we exit here?
    }
    int ret = env->ThrowNew(exception, s.c_str());
    if (ret != 0)
        XDEMessage::log(XDEMessage::WARNING,
                        "Error value returned when throwing exception.");
}

// Routines to convert data from C++ types to jni to java objects

jobjectArray createJavaDoubleMatrix(JNIEnv *env, const TDoubleMatrix &values) {
    // TODO: Test with values.size()=0
    jclass doubleArrClass = env->FindClass("[D");
    int nRows = values.size1();
    int nCols = values.size2();
    jobjectArray ret =
        (jobjectArray)env->NewObjectArray(nRows, doubleArrClass, NULL);
    if (!ret) throw JNIOutOfMemoryException();

    double *rowData = new double[nCols];
    if (!rowData) {
        throw JNIOutOfMemoryException();
    }

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nCols; j++) rowData[j] = values(i, j);
        jdoubleArray newRow = env->NewDoubleArray(nCols);
        if (!newRow) throw JNIOutOfMemoryException();
        env->SetDoubleArrayRegion(newRow, 0, nCols, rowData);
        env->SetObjectArrayElement(ret, i, newRow);
        env->DeleteLocalRef(newRow);
    }
    delete[] rowData;
    return ret;
}


jobjectArray createJavaVector(JNIEnv *env, const vector<string> &values) {
    jclass stringClass = env->FindClass("Ljava/lang/String;");
    assert(stringClass != NULL);
    jobjectArray ret = env->NewObjectArray(values.size(), stringClass, NULL);
    for (unsigned i = 0; i < values.size(); i++) {
        jstring jstr = env->NewStringUTF(values[i].c_str());
        env->SetObjectArrayElement(ret, i, jstr);
    }

    return ret;
}

jdoubleArray createJavaVector(JNIEnv *env, const TDoubleVector &values) {
    jclass doubleArrClass = env->FindClass("[D");
    assert(doubleArrClass != NULL);
    jdoubleArray ret = env->NewDoubleArray(values.size());
    double *dataPtr = new double[values.size()];
    for (unsigned i = 0; i < values.size(); i++) dataPtr[i] = values[i];
    env->SetDoubleArrayRegion(ret, 0, values.size(), dataPtr);
    delete[] dataPtr;
    return ret;
}

jdoubleArray createJavaVector(JNIEnv *env, const vector<double> &values) {
    jclass doubleArrClass = env->FindClass("[D");
    assert(doubleArrClass != NULL);

    double *dataPtr = vectorToArray(values);
    jdouble *jdataPtr = new jdouble[values.size()];
    for (unsigned i = 0; i < values.size(); i++)
        jdataPtr[i] = (jdouble)dataPtr[i];

    jdoubleArray ret = env->NewDoubleArray(values.size());
    env->SetDoubleArrayRegion(ret, 0, values.size(), jdataPtr);
    delete[] dataPtr;
    delete[] jdataPtr;
    return ret;
}

// TODO: Clean this one up...
jintArray createJavaVector(JNIEnv *env, const vector<int> &values) {
    jclass intArrClass = env->FindClass("[I");
    assert(intArrClass != NULL);
    jintArray ret = env->NewIntArray(values.size());
    int *dataPtr = vectorToArray(values);
    jint *jintPtr = new jint[values.size()];
    for (unsigned i = 0; i < values.size(); i++) jintPtr[i] = dataPtr[i];
    env->SetIntArrayRegion(ret, 0, values.size(), jintPtr);
    delete[] dataPtr;
    delete[] jintPtr;
    return ret;
}

jintArray createJavaVector(JNIEnv *env, const vector<unsigned> &values) {
    jclass intArrClass = env->FindClass("[I");
    assert(intArrClass != NULL);
    jintArray ret = env->NewIntArray(values.size());
    unsigned *dataPtr = vectorToArray(values);
    jint *jintPtr = new jint[values.size()];
    for (unsigned i = 0; i < values.size(); i++) jintPtr[i] = (jint)dataPtr[i];
    env->SetIntArrayRegion(ret, 0, values.size(), jintPtr);
    delete[] dataPtr;
    delete[] jintPtr;
    return ret;
}

jlongArray createJavaVector(JNIEnv *env, const vector<jlong> &values) {
    jclass longArrClass = env->FindClass("[I");
    assert(longArrClass != NULL);
    jlongArray ret = env->NewLongArray(values.size());
    jlong *dataPtr = vectorToArray(values);
    jlong *jlongPtr = new jlong[values.size()];
    for (unsigned i = 0; i < values.size(); i++)
        jlongPtr[i] = (jlong)dataPtr[i];
    env->SetLongArrayRegion(ret, 0, values.size(), jlongPtr);
    delete[] dataPtr;
    delete[] jlongPtr;
    return ret;
}

/**
 * Create a vector<string> from a java array of Strings.
 */
vector<string> createStringVector(JNIEnv *env, const jobjectArray &strings) {
    vector<string> ret;
    if (strings == NULL) return ret;

    int size = env->GetArrayLength(strings);
    ret.resize(size);

    for (int i = 0; i < size; i++) {
        jstring obj = (jstring)env->GetObjectArrayElement(strings, i);
        if (obj == NULL) {
            ret[i] = "";
        } else {
            const char *s = env->GetStringUTFChars(obj, NULL);
            ret[i] = string(s);
            env->ReleaseStringUTFChars(obj, s);
        }
    }

    return ret;
}

// TODO :: Use templates here? Probably not because of GetIntArrayRegion
// function
vector<int> createIntVector(JNIEnv *env, const jintArray &values) {
    vector<int> ret;
    if (values == NULL) return ret;

    int size = env->GetArrayLength(values);
    vector<jint> jintValues(size);
    ret.resize(size);

    jint *intPtr = new jint[size];
    env->GetIntArrayRegion(values, 0, size, intPtr);
    jintValues.assign(intPtr, intPtr + size);

    for (int i = 0; i < size; i++) ret[i] = (int)jintValues[i];

    delete[] intPtr;
    return ret;
}

vector<unsigned> createUnsignedVector(JNIEnv *env, const jintArray &values) {
    vector<unsigned> ret;
    if (values == NULL) return ret;

    int size = env->GetArrayLength(values);
    vector<jint> jintValues(size);
    ret.resize(size);

    jint *intPtr = new jint[size];
    env->GetIntArrayRegion(values, 0, size, intPtr);
    jintValues.assign(intPtr, intPtr + size);

    for (int i = 0; i < size; i++) ret[i] = (unsigned)jintValues[i];

    delete[] intPtr;
    return ret;
}

vector<double> createDoubleVector(JNIEnv *env, const jdoubleArray &values) {
    vector<double> ret;
    if (values == NULL) return ret;

    int size = env->GetArrayLength(values);
    ret.resize(size);
    double *dblPtr = new double[size];
    env->GetDoubleArrayRegion(values, 0, size, dblPtr);
    ret.assign(dblPtr, dblPtr + size);
    delete[] dblPtr;
    return ret;
}


/**
 * Create a C++ TDoubleMatrix object from a java matrix of doubles.
 *
 * Note that the java matrix contains primitive double types and not
 * Double objects.
 */

TDoubleMatrix *createDoubleMatrix(JNIEnv *env,
                                  const jobjectArray &doubleMatrix) {
    // First figure out the dimensions of the matrix.
    if (doubleMatrix == NULL) return new TDoubleMatrix();

    jsize totalEntries = env->GetArrayLength(doubleMatrix);
    if (totalEntries == 0) return new TDoubleMatrix();

    jdoubleArray rowData =
        (jdoubleArray)env->GetObjectArrayElement(doubleMatrix, 0);
    jsize nCols = env->GetArrayLength(rowData);
    jsize nRows = totalEntries;
    TDoubleMatrix *ret = new TDoubleMatrix(nRows, nCols);
    vector<double> row;
    for (int i = 0; i < nRows; i++) {
        jdoubleArray rowData =
            (jdoubleArray)env->GetObjectArrayElement(doubleMatrix, i);
        row = createDoubleVector(env, rowData);
        for (int j = 0; j < nCols; j++) {
            (*ret)(i, j) = row[j];
        }
        // ret->replaceRow(i,  getTVector(row));
    }
    return ret;
}

TDoubleVector createTDoubleVector(JNIEnv *env, const jdoubleArray &values) {
    TDoubleVector ret;
    if (values == NULL) return ret;
    int size = env->GetArrayLength(values);
    ret.resize(size);

    double *dblPtr = new double[size];
    env->GetDoubleArrayRegion(values, 0, size, dblPtr);

    for (int i = 0; i < size; i++) ret[i] = dblPtr[i];
    // ret.insert(0, size, dblPtr);
    delete[] dblPtr;
    return ret;
}


/**
 * Methods to query the system about the state of the memory.
 */

double getMaxMemory(JNIEnv *env) {
    // Got a runtime object. Now can get memory
    jobject runtimeObject = getRuntimeObject(env);

    jclass runtime = env->FindClass("java/lang/Runtime");
    assert(runtime != NULL);

    jmethodID mid = env->GetMethodID(runtime, "maxMemory", "()J");
    long maxMem = env->CallLongMethod(runtimeObject, mid);
    return (double)maxMem;
}

double getUsedMemory(JNIEnv *env) {
    // Got a runtime object. Now can get memory
    jobject runtimeObject = getRuntimeObject(env);

    jclass runtime = env->FindClass("java/lang/Runtime");
    assert(runtime != NULL);

    jmethodID mid = env->GetMethodID(runtime, "totalMemory", "()J");
    long totalMem = env->CallLongMethod(runtimeObject, mid);
    return (double)totalMem;
}

double getAvailableMemory(JNIEnv *env) {
    // Got a runtime object. Now can get memory
    jobject runtimeObject = getRuntimeObject(env);

    jclass runtime = env->FindClass("java/lang/Runtime");
    assert(runtime != NULL);

    jmethodID mid = env->GetMethodID(runtime, "freeMemory", "()J");
    long freeMem = env->CallLongMethod(runtimeObject, mid);
    return (double)freeMem;
}

jobject getRuntimeObject(JNIEnv *env) {
    jclass runtime = env->FindClass("java/lang/Runtime");
    assert(runtime != NULL);
    jmethodID mid =
        env->GetStaticMethodID(runtime, "getRuntime", "()Ljava/lang/Runtime;");
    assert(mid != NULL);

    jobject runtimeObject = env->CallStaticObjectMethod(runtime, mid);
    assert(runtimeObject != NULL);
    return runtimeObject;
}

boost::any getNativePointer(JNIEnv *env, jobject obj) {
    // assert(obj != NULL);
    if (obj == NULL) {
        throwSystemException(
            env, "Trying to get the native pointer of a null object.");
        return NULL;
    }
    jclass cls = env->GetObjectClass(obj);
    assert(cls != NULL);
    jmethodID mid = (env)->GetMethodID(cls, "getObjectIndex", "()J");
    assert(mid != NULL);

    unsigned indx =
        boost::numeric_cast<unsigned>(env->CallLongMethod(obj, mid));
    return NativeObjectManager::getInstance()->getPtr(indx);
}

/**
 * Generic method called by both getControlParameters and
 * getAllcontrolParameters. getControlParameters can be redefined by
 * the subclasses of Algorithm.
 *
 * Probably a better way of doing this, but it would involve
 * understanding how iheritance behaves in JNI. Until I know, this
 * will have to do.
 */

jobjectArray getParameters(JNIEnv *env, XDEAlgorithm *algm) {
    try {
        vector<GeneralParameterPtr> controlParams =
            algm->getControlParameters();

        jclass generalParameterClass =
            env->FindClass("edu/rochester/urmc/cbim/jni/JGeneralParameter");
        assert(generalParameterClass != NULL);

        unsigned nbElems = controlParams.size();
        jobjectArray ret =
            env->NewObjectArray(nbElems, generalParameterClass, NULL);

        for (unsigned i = 0; i < nbElems; i++) {
            jobject jsymbol = createDefaultJavaObject(
                env, "edu/rochester/urmc/cbim/jni/JGeneralParameter",
                controlParams[i]);
            env->SetObjectArrayElement(ret, i, jsymbol);
        }
        return ret;
    }
    JNI_EXCEPTIONS;
    return NULL;
}

jobjectArray getAvailableMethods(JNIEnv *env, AlgorithmFactory &af) {
    vector<string> methods = af.getKnownMethods();
    return createJavaVector(env, methods);
}

jboolean isAttributeDefined(JNIEnv *env, AlgorithmFactory &af, jstring method,
                            jstring attribute) {
    try {
        const char *met = env->GetStringUTFChars(method, NULL);
        const char *attr = env->GetStringUTFChars(attribute, NULL);
        bool b = af.isAttributeDefined(met, attr);
        env->ReleaseStringUTFChars(attribute, attr);
        env->ReleaseStringUTFChars(method, met);
        return b;
    }
    JNI_EXCEPTIONS;
    return false;
}

EstimationParameterSetPtr createEstimationParameterSet(
    JNIEnv *env, jobjectArray JNIParameters) {
    try {
        EstimationParameterSetPtr parameterSet(new EstimationParameterSet());

        unsigned nbParams =
            boost::numeric_cast<unsigned>(env->GetArrayLength(JNIParameters));

        jclass parameterValuesID =
            env->FindClass("edu/rochester/urmc/cbim/jni/ParameterValue");
        assert(parameterValuesID != NULL);

        jfieldID nameID =
            env->GetFieldID(parameterValuesID, "name", "Ljava/lang/String;");
        jfieldID valueID = env->GetFieldID(parameterValuesID, "value", "D");
        jfieldID minID = env->GetFieldID(parameterValuesID, "min", "D");
        jfieldID maxID = env->GetFieldID(parameterValuesID, "max", "D");
        jfieldID estimatedID =
            env->GetFieldID(parameterValuesID, "estimated", "Z");

        assert(nameID != NULL && valueID != NULL && minID != NULL &&
               maxID != NULL && estimatedID != NULL);

        for (unsigned i = 0; i < nbParams; i++) {
            jobject parameterValues =
                env->GetObjectArrayElement(JNIParameters, i);

            double value = boost::numeric_cast<double>(
                env->GetDoubleField(parameterValues, valueID));
            double minValue = boost::numeric_cast<double>(
                env->GetDoubleField(parameterValues, minID));
            double maxValue = boost::numeric_cast<double>(
                env->GetDoubleField(parameterValues, maxID));
            bool estimated = boost::numeric_cast<bool>(
                env->GetBooleanField(parameterValues, estimatedID));

            jstring nameObject =
                (jstring)(env->GetObjectField(parameterValues, nameID));
            const char *nameStr = env->GetStringUTFChars(nameObject, NULL);
            parameterSet->addParameter(EstimationSymbolAttributes(
                string(nameStr), value, minValue, maxValue, estimated));
            env->ReleaseStringUTFChars(nameObject, nameStr);
        }
        return parameterSet;
    }
    JNI_EXCEPTIONS;
    throw XDEException("What is happening here?");
}
