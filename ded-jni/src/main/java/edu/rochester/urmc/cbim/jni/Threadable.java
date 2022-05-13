
package edu.rochester.urmc.cbim.jni;


public abstract class Threadable extends XDEBase {

    // Used by C++ code. Do not remove.
    private long threadPtr;

    public native void interruptThread();
    public native void pauseThread();
    public native void resumeThread();

    Threadable() {}
}
