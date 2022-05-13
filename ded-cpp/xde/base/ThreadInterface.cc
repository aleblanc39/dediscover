#include <base/ThreadInterface.h>
#include <base/XDEMessage.h>

ThreadInterface::ThreadInterface() {
    sleepTime = 1;
    threadStatus = RUN;
}

void ThreadInterface::interruptThread() { setThreadStatus(INTERRUPT); }

void ThreadInterface::pauseThread() { setThreadStatus(PAUSE); }

void ThreadInterface::resumeThread() { setThreadStatus(RUN); }

ThreadInterface::threadAction ThreadInterface::getThreadStatus() const {
    return threadStatus;
}

void ThreadInterface::setThreadStatus(threadAction a) { threadStatus = a; }

long ThreadInterface::getSleepTime() const { return sleepTime; }

void ThreadInterface::setSleepTime(long t) { sleepTime = t; }

void ThreadInterface::handleThread() {
    while (threadStatus == PAUSE) {
         XDEMessage::log(XDEMessage::INFO, "Thread is paused");
        _SLEEP(sleepTime);
    }
    if (threadStatus == INTERRUPT) {
        XDEMessage::log(XDEMessage::INFO, "Thread is interrupted. Will throw exception.");
        threadStatus = RUN;
        throw StopThread();
    }
}
