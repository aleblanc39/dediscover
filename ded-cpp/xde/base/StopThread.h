#ifndef STOPTHREAD_H
#define STOPTHREAD_H
#include <stdexcept>

/**
 * \brief Object to throw when a thread is to stop,
 *
 * This is a simple object that will be thrown when a signal to stop a
 * thread has been sent. A typical scenario will be in conjunction
 * with jni, where a class implementing native routine will be part of
 * a thread and will receive a message to stop the execution of some 
 * computation.
 */
class StopThread: public std::runtime_error {

 public:
    StopThread() : std::runtime_error("Interrupt from user"){}

};



#endif
