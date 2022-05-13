
package edu.rochester.urmc.cbim.utility.exception;

public class XDEInternalErrorRTException extends ErrorRTException {
    private static final long serialVersionUID = 1L;

    public XDEInternalErrorRTException() {
	super("Internal error exception (XDE): ");
    }

    public XDEInternalErrorRTException(String msg) {
	super("Internal error exception (XDE): " + msg);
    }

    public XDEInternalErrorRTException(String msg, Exception e) {
	super("Internal error exception (XDE): " + msg, e);
    }

    public XDEInternalErrorRTException(String message, Throwable cause) {
	super(message, cause);
    }

}
