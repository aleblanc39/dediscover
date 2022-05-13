#include <edu_rochester_urmc_cbim_jni_LoggerThread.h>
#include <iostream>
#include <string>
#include <cassert>
#include <base/XDEMessage.h>
#include <base/CBIMDefinitions.h>
//#include <atomic_ops.h>
#include <jni.h> // Eclipse gives an error if this is not included.

using namespace std;

#ifdef WIN32
#undef ERROR  // Windows defines an ERROR macro in wingdi.h.  Undefine here to avoid compilation errors
#endif

static int msgBufferSize = 1000;
static string *messageBuffer = NULL;
static int *messageLogLevel = NULL;
static int nbMsgIn = 0;
static int nbMsgOut = 0;
static bool stopThread = false;

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
using namespace boost::interprocess;

void XDELogMessage(XDEMessage::MsgLevel level, const string &msg);
//static volatile AO_TS_t  lock = 0;

static boost::interprocess::interprocess_mutex mutex;

/*
 * Class:     edu_rochester_urmc_cbim_jni_LoggerThread
 * Method:    readXDEMessages
 * Signature: (Ljava/util/logging/Logger;)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_LoggerThread_readXDEMessages
(JNIEnv *env, jobject obj, jobject javaLogger) {

    XDEMessage::setLogFunction(&XDELogMessage);
    messageBuffer = new string[msgBufferSize];
    messageLogLevel = new int[msgBufferSize];
    // TODO Make sure memory is allocated.
    jclass loggerClass = env->FindClass("java/util/logging/Logger");
    assert(loggerClass != NULL);

    static const char *methodName[] = {"",
				 "severe",  // ERROR
				  "warning", // WARNING
				  "info",    // INFO
				  "config",  // DEBUG
				  "fine",    // DEBUG1
				  "finer",   // DEBUG2
				  "finest",  // DEBUG3
				  "finest",  // DEBUG4
				  "finest"};  // DEBUG5

    // Map the XDEMessage log level to the java logger level

    jmethodID loggingMethod;
    stopThread = false;
    string nextMsg;
    int index;
    while (!stopThread) {
	while (nbMsgOut < nbMsgIn) {
	    index = nbMsgOut%msgBufferSize;
	    loggingMethod = env->GetMethodID(loggerClass,
					     methodName[messageLogLevel[index]],
					     "(Ljava/lang/String;)V");
	    nextMsg = messageBuffer[index];
	    env->CallVoidMethod(javaLogger, loggingMethod, env->NewStringUTF(nextMsg.c_str()));
	    nbMsgOut++;
	}
	_SLEEP(1);
    }

    // Need to define a critical region to ensure that no message is written to the buffers
    // after they have been deleted.
    // while (AO_test_and_set_full(&lock)) {
    // 	//cerr << "Spinning on deleting message buffers\n";
    // }

    scoped_lock<interprocess_mutex> lock(mutex);
    delete []messageBuffer;
    delete []messageLogLevel;
    //lock = 0;
}



/*
 * Class:     edu_rochester_urmc_cbim_jni_LoggerThread
 * Method:    setLoggingLevel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_LoggerThread_setLoggingLevel
(JNIEnv *env, jclass cls, jint level) {
    XDEMessage::setLogLevel((XDEMessage::MsgLevel)level);
}


void XDELogMessage(XDEMessage::MsgLevel level, const string &msg) {

    //    assert(messageLogLevel != NULL && messageBuffer != NULL);

    if (messageLogLevel == NULL ||  messageBuffer == NULL) {
	cerr << "Failed to log...\n";
	return;
    }
    int index = nbMsgIn%msgBufferSize;


    // Put code in critical region to endure that the buffer are not deleted by the time
    // they are written to.
    // while (AO_test_and_set_full(&lock)) {
    // 	//cerr << "Spinning on deleting message buffers\n";
    // }
    scoped_lock<interprocess_mutex> lock(mutex);
    if (messageBuffer != NULL) {
	messageLogLevel[index] = (int) level;
	messageBuffer[index] = msg;
	nbMsgIn++;
    }
    //lock = 0;
}

static XDEMessage::MsgLevel convertFromMslib[] = {
	    XDEMessage::DEBUG3,
	    XDEMessage::DEBUG2,
	    XDEMessage::DEBUG1,
	    XDEMessage::DEBUG,
	    XDEMessage::INFO,
	    XDEMessage::WARNING,
	    XDEMessage::XDE_ERROR};


/*
 * Class:     edu_rochester_urmc_cbim_jni_LoggerThread
 * Method:    sendTerminationMessage
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_edu_rochester_urmc_cbim_jni_LoggerThread_sendTerminationMessage
(JNIEnv *, jclass) {

    // Give some time to remove all pending messages.
    _SLEEP(1);
    stopThread = true;
    XDEMessage::resetFunction();
    _SLEEP(1);
}
