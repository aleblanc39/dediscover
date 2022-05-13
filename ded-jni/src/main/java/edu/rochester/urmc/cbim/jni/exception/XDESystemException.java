
package edu.rochester.urmc.cbim.jni.exception;

public class XDESystemException extends RuntimeException {
	static final long serialVersionUID = 5079422959038540540L;

	public XDESystemException() {
		super();
	}

	public XDESystemException(String arg0) {
		super(arg0);
	}

	/**
	 * @param message
	 * @param cause
	 */
	public XDESystemException(String message, Throwable cause) {
		super(message, cause);
	}

	/**
	 * @param cause
	 */
	public XDESystemException(Throwable cause) {
		super(cause);
	}

}
