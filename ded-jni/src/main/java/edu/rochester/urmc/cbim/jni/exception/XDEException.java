
package edu.rochester.urmc.cbim.jni.exception;

/**
 * XDEException class.
 * 
 * This class will mirror the XDEException from the C++ XDE code.
 */

public class XDEException extends RuntimeException {
	public static final long serialVersionUID = 24362462L;

	public XDEException() {
	}

	public XDEException(String msg) {
		super(msg);
	}
}
