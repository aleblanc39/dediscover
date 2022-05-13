
package edu.rochester.urmc.cbim.utility.exception;

public class ErrorRTException extends RuntimeException {

    private static final long serialVersionUID = -6343838423960369443L;

    public ErrorRTException() {
	super();
    }

    public ErrorRTException(String msg) {
	super(msg);
    }

    public ErrorRTException(String message, Throwable cause) {
	super(message, cause);
    }

    /**
     * @param e
     * @param string
     */
    public ErrorRTException(String msg, Exception e) {
	super(msg, e);
    }
}
