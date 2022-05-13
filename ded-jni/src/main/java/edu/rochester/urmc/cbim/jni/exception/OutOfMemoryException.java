
package edu.rochester.urmc.cbim.jni.exception;


public class OutOfMemoryException extends Exception {

	private static final long serialVersionUID = -1192485185865332073L;

	public OutOfMemoryException() {
	}

	public OutOfMemoryException(String msg) {
		super(msg);
	}

    public OutOfMemoryException(String msg, double free, double max, double required ) {
		super(msg);
		memoryUsed = max - free;
		maxMemory = max;
		memoryRequired = required;
	       
	}

    public double getMemoryUsed() {return memoryUsed;}
    public double getMaxMemory() {return maxMemory;}
    public double getMemoryRequired() {return memoryRequired;}

    private double memoryUsed = -1;
    private double maxMemory = -1;
    private double memoryRequired = -1;


}
