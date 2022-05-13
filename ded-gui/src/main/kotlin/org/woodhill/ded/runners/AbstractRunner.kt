package org.woodhill.ded.runners

import edu.rochester.urmc.cbim.jni.JDDEModel

open abstract class AbstractRunner protected constructor(protected open val model: JDDEModel?) {

    protected var t: Thread? = null

    fun interruptThread() {
        println("Interrupting thread")
        model?.interruptThread()
    }

//    fun isThreadRunning(): Boolean {
//        return if (t == null) {
//            false
//        } else t!!.isAlive
//    }

    fun pauseThread() {
        println("AbstractRunner: pausing the thread for $model")
        model!!.pauseThread()
    }

    fun resumeThread() {
        println("AbstractRunner: resuming the thread for $model")

        model?.resumeThread()
    }
}

