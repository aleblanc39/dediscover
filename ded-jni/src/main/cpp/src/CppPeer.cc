#include <boost/numeric/conversion/cast.hpp>
#include <cassert>
#include <iostream>

#include "NativeObjectManager.h"
#include "edu_rochester_urmc_cbim_jni_util_CppPeer.h"

/*
 * Class:     edu_rochester_urmc_cbim_jni_util_CppPeer
 * Method:    releaseNativeObject
 * Signature: (J)V
 */

JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_util_CppPeer_releaseNativeObject(
    JNIEnv *env, jobject, jlong objectIndex) {
    // Turns out exception not really thrown, but doesn't hurt. Looks
    // like there are cases (JObjectiveFunction) where no native object is
    // created.

    try {
        if (objectIndex < 0) {
            jclass exception = env->FindClass(
                "edu/rochester/urmc/cbim/jni/exception/XDESystemException");
            env->ThrowNew(exception, "An object has a negative index..");
            return;
        }
        unsigned indx = boost::numeric_cast<unsigned>(objectIndex);
        NativeObjectManager::getInstance()->removePtr(indx);
    } catch (...) {
        std::cerr << "Some exception caught in CppPeer::releaseNativeObject\n";
    }
}
