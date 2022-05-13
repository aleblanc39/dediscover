#ifndef THREADINTERFACE_H
#define THREADINTERFACE_H
#include <iostream>
#include<memory>
#include <base/StopThread.h>
#include <base/CBIMDefinitions.h>
/**
 * The is no standard sleep() function in C++. g++ and MSVC handle it
 * differently.
 */


class ThreadInterface;

typedef std::shared_ptr<ThreadInterface> ThreadInterfacePtr;

/**
 * \brief Class to allow the interruption of computation in C++ code
 *
 * This class allows C++ class inheriting it to perform thread-like
 * operations. We use in in conjunction with Java threads where one
 * thread can call a method to interrupt/pause/resume, and the process
 * periodically check on the status do decide on whether to stop or
 * resume computation.
 */

class ThreadInterface {

 public:
    typedef enum {RUN, PAUSE, INTERRUPT} threadAction;
    ThreadInterface();


    void interruptThread();
    void pauseThread();
    void resumeThread();



    threadAction getThreadStatus () const;

    void setThreadStatus(threadAction a);

    long getSleepTime() const;
    void setSleepTime(long t);
    void handleThread();

 protected:
   threadAction threadStatus;
 private:

    
    long sleepTime;

};





#endif
