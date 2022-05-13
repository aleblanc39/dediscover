#ifndef JNIUTIL_H
#define JNIUTIL_H

#include <JNIOutOfMemoryException.h>
#include <algorithms/AlgorithmTemplates.h>
#include <algorithms/XDEAlgorithm.h>
#include <base/ThreadInterface.h>
#include <base/XDEBase.h>
#include <base/XDEException.h>
#include <base/XDEProgrammingException.h>
#include <base/XDEUtil.h>
#include <base/XDEBlas.h>
#include <extra_parser_functions/TVFunctionException.h>
#include <jni.h>
#include <objectfunction/DataGenerator.h>
#include <parameters/ParameterRangeException.h>

#include <boost/cast.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <string>
#include <vector>

#include "NativeObjectManager.h"

jobject getRuntimeObject(JNIEnv *env);

// Collapse into one function and use RTTI?
void throwOutOfMemoryException(JNIEnv *env, std::string s);
void throwXDEException(JNIEnv *env, const XDEException &e);
void throwXDEException(JNIEnv *env, std::string s);
void throwInterruptException(JNIEnv *env, std::string s);
void throwParserException(JNIEnv *env, std::string s);
void throwParameterRangeException(JNIEnv *env, ParameterRangeException *e);
void throwSystemException(JNIEnv *env);
void throwSystemException(JNIEnv *env, std::string s);

jlong getObjectIndex(JNIEnv *env, jobject obj);


EstimationParameterSetPtr createEstimationParameterSet(
    JNIEnv *env, jobjectArray JNIParameters);

boost::any getNativePointer(JNIEnv *env, jobject obj);

/**
 * Create a link between the native C++ object and the parent java object.
 * The link is maintained by the NativeObjectManager C++ singleton that keeps
 * an array where the base of the parent object (CppPeer) has a pointer to
 * an array location, and the array contains boost::any abjects that point to
 * the C++ object.
 * For some object need to store the pointer to a base class (currently Algorithm
 * and ThreadInterface)
 *
 * Since we allow for native objects to be replaced, must first remove previous one
 * otherwise we create memory leaks.
*/
template <typename T>
void assignTheNativeObject(JNIEnv *env, jobject obj, T ptr) {
    jclass cls = env->GetObjectClass(obj);
    assert(cls != NULL);
    jmethodID setObjectIndex = (env)->GetMethodID(cls, "setObjectIndex", "(J)V");
    assert(setObjectIndex != NULL);

    try {

        jlong oldIndex = getObjectIndex(env, obj);
        if (oldIndex >= 0) {
            unsigned oi =  boost::numeric_cast<unsigned>(oldIndex);
            NativeObjectManager::getInstance() -> removePtr(oi);
        }

        unsigned indx = NativeObjectManager::getInstance()->addPtr(boost::any(ptr));
        jlong jindx = boost::numeric_cast<jlong>(indx);

        (env)->CallVoidMethod(obj, setObjectIndex, jindx);
        jmethodID basePtr = env->GetMethodID(cls, "setBasePtr", "(J)V");
        assert(basePtr != NULL);
        env->CallVoidMethod(obj, basePtr, (jlong)((XDEBase *)ptr.get()));
    } catch (std::string s) {
        throwXDEException(env, s);
    }
    

    // Assign the algorithm ptr. In some cases it makes no sense, but it will
    // not be used.
    try {
        XDEAlgorithm *algmPtr =
            boost::polymorphic_cast<XDEAlgorithm *>(ptr.get());
        // cerr << "Cast to algm ok: " << (long) algmPtr << "\n";
        jfieldID algmID = env->GetFieldID(cls, "algorithmPtr", "J");
        assert(algmID != NULL);
        env->SetLongField(obj, algmID, (long)(algmPtr));
    } catch (...) {
        // Come here if this is not a Algorithm class
        // cerr << "Exception casting to algm. Probably ok.\n";
    }

    // Ditto for the thread pointer.

    try {
        ThreadInterface *threadPtr =
            boost::polymorphic_cast<ThreadInterface *>(ptr.get());
        // cerr << "Cast to thread ok: " << (long) threadPtr << "\n";
        jfieldID threadID = env->GetFieldID(cls, "threadPtr", "J");
        //	assert(threadID != NULL);
        env->SetLongField(obj, threadID, (long)(threadPtr));
    } catch (...) {
        // Come here if this is not a Algorithm class
        // cerr << "Exception casting to thread. Probably ok.\n";
    }
}

/**
 * Creates the default java object for the given class. It will invoke
 * the constructor with a boolean argument to indicate not to create a
 * native object. This is to be used when the java object is created
 * from withing the JNI C++ code.
 */

template <typename T>
jobject createDefaultJavaObject(JNIEnv *env, std::string className,
                                T sharedPtr) {
    // std::cerr << "JNIUtil.createDefaultJavaObject: class name: " << className
    // << std::endl;

    if (sharedPtr.get() == NULL) return NULL;
    jclass classID = env->FindClass(className.c_str());
    if (classID == NULL) {
        std::cerr << "Looking at class: " << className << std::endl;
    }
    XDE_ASSERT(classID != NULL);
    jmethodID constructorID = env->GetMethodID(classID, "<init>", "()V");
    assert(constructorID != NULL);

    jobject jresult = env->NewObject(classID, constructorID);
    // std::cerr << "JNIUtil.createDefaultJavaObject: jresult is: " << jresult
    // << std::endl;
    XDE_ASSERT(jresult != NULL);
    // std::cerr << "passed the assertion\n";
    assignTheNativeObject(env, jresult, sharedPtr);
    return jresult;
}

jobjectArray getAvailableMethods(JNIEnv *env, AlgorithmFactory &af);

/**
 * Here T should be some smart ptr class.
 */

template <typename T>
jobjectArray createJavaSmartPtrArray(JNIEnv *env, const std::vector<T> &params,
                                     std::string className) {
    jclass javaClass = env->FindClass(className.c_str());
    assert(javaClass != NULL);

    unsigned nbElems = params.size();
    jobjectArray ret = env->NewObjectArray(nbElems, javaClass, NULL);

    for (unsigned i = 0; i < nbElems; i++) {
        jobject jsymbol = createDefaultJavaObject(env, className, params[i]);
        env->SetObjectArrayElement(ret, i, jsymbol);
    }
    return ret;
}

jlong getBasePtr(JNIEnv *env, jobject obj);
jlong getAlgorithmPtr(JNIEnv *env, jobject obj);
jlong getThreadPtr(JNIEnv *env, jobject obj);
jlong getObjectIndex(JNIEnv *env, jobject obj);

double getMaxMemory(JNIEnv *env);
double getUsedMemory(JNIEnv *env);
double getAvailableMemory(JNIEnv *env);

std::vector<std::string> createStringVector(JNIEnv *env,
                                            const jobjectArray &strings);
std::vector<int> createIntVector(JNIEnv *env, const jintArray &strings);
std::vector<unsigned> createUnsignedVector(JNIEnv *env,
                                           const jintArray &strings);
std::vector<double> createDoubleVector(JNIEnv *env, const jdoubleArray &values);
TDoubleVector createTDoubleVector(JNIEnv *env, const jdoubleArray &values);

TDoubleMatrix *createDoubleMatrix(JNIEnv *env,
                                  const jobjectArray &doubleMatrix);

// Routines to convert data from C++ types to jni to java objects

jobjectArray createJavaDoubleMatrix(JNIEnv *env, const TDoubleMatrix &values);

jobjectArray createJavaVector(JNIEnv *env,
                              const std::vector<std::string> &values);
jintArray createJavaVector(JNIEnv *env, const std::vector<int> &values);
jintArray createJavaVector(JNIEnv *env, const std::vector<unsigned> &values);
jlongArray createJavaVector(JNIEnv *env, const std::vector<jlong> &values);
jdoubleArray createJavaVector(JNIEnv *env, const TDoubleVector &values);
jdoubleArray createJavaVector(JNIEnv *env, const std::vector<double> &values);

jobjectArray getParameters(JNIEnv *env, XDEAlgorithm *algm);

#define JNI_EXCEPTIONS                                                        \
    catch (JNIOutOfMemoryException e) {                                       \
        throwOutOfMemoryException(env, e.getMsg());                           \
    }                                                                         \
    catch (std::bad_alloc &) {                                                \
        throwOutOfMemoryException(env,                                        \
                                  "bad_alloc thrown, likely from XDE code."); \
    }                                                                         \
    catch (XDEProgrammingException e) {                                       \
        std::cerr << "Programming exception: " << e.getMsg() << std::endl;    \
        throwSystemException(env, e.getMsg());                                \
    }                                                                         \
    catch (XDEException & e) {                                                \
        throwXDEException(env, e);                                            \
    }                                                                         \
    catch (boost::numeric::negative_overflow e) {                             \
        throwSystemException(env, "Numeric overflow");                        \
    }                                                                         \
    catch (boost::bad_any_cast b) {                                           \
        std::cerr << "Bad cast: " << b.what() << std::endl;                   \
        throwSystemException(env, "Boost cast exception");                    \
    }                                                                         \
    catch (TVFunctionException e) {                                           \
        throwXDEException(env, e.getMsg());                                   \
    }                                                                         \
    catch (...) {                                                             \
        std::cerr << "The three dots :( \n";                                  \
        throwSystemException(env);                                            \
    }

jboolean isAttributeDefined(JNIEnv *env, AlgorithmFactory &af, jstring method,
                            jstring attribute);

#endif  // JNUTIL_H
