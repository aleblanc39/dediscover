#include <JNIUtil.h>
#include <base/ThreadInterface.h>
#include <edu_rochester_urmc_cbim_jni_Threadable.h>
#include <model/XDEModel.h>

#include <cassert>
#include <iostream>

using namespace std;

/*
 * Class:     edu_rochester_urmc_cbim_jni_Threadable
 * Method:    interruptThread
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_edu_rochester_urmc_cbim_jni_Threadable_interruptThread(JNIEnv *env,
                                                            jobject obj) {
    try {
        XDEMessage::log(XDEMessage::DEBUG2,
                        "Threadable.cc: Interrupting thread.\n");
        ThreadInterface *w2 = (ThreadInterface *)getThreadPtr(env, obj);
        w2->interruptThread();
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_Threadable
 * Method:    suspendThread
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_Threadable_pauseThread(
    JNIEnv *env, jobject obj) {
    try {
        XDEMessage::log(XDEMessage::DEBUG2, "Threadable.cc: Pausing thread.\n");
        ThreadInterface *w2 = (ThreadInterface *)getThreadPtr(env, obj);
        w2->pauseThread();
    }
    JNI_EXCEPTIONS;
}

/*
 * Class:     edu_rochester_urmc_cbim_jni_Threadable
 * Method:    resumeThread
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_Threadable_resumeThread(
    JNIEnv *env, jobject obj) {
    try {
        XDEMessage::log(XDEMessage::DEBUG2,
                        "Threadable.cc: Resuming thread.\n");
        ThreadInterface *w2 = (ThreadInterface *)getThreadPtr(env, obj);
        w2->resumeThread();
    }
    JNI_EXCEPTIONS;
}
