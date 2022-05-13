
package edu.rochester.urmc.cbim.jni.exception;

/**
 * InterruptException class.
 *
 * This class is thrown to the java thread classes after the C++ has
 * stopped the execution to signal to the java class that the thread
 * can be interrupted.
 */

public class InterruptException extends Exception {
    public static final long serialVersionUID = 24362462L;
    public InterruptException() {}
    public InterruptException(String msg) {}
    
}
