package edu.rochester.urmc.cbim.jni.exception;

public class ParameterRangeException extends XDEException {

	/**
     * 
     */
	private static final long serialVersionUID = 1L;

	public ParameterRangeException() {
		super("Error in parameter range.");
	}

	public ParameterRangeException(String s) {
		super(s);
	}

	/**
	 * @param min2
	 * @param value2
	 * @param max2
	 */
	public ParameterRangeException(double min, double value, double max) {
		super("Error in parameter range (min,value,max) (" + min + "," + value + "," + max + ")");
	}

	/**
	 * @param name2
	 * @param min2
	 * @param value2
	 * @param max2
	 */
	public ParameterRangeException(String name, double min, double value, double max) {
		super("Error in parameter range (name,min,value,max) (" + name + ", " + min + "," + value + "," + max + ")");
	}

	String name;
	String min;
	String max;
	String value;
	boolean hasMinValue;
	boolean hasMaxValue;

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getMin() {
		return min;
	}

	public void setMin(String min) {
		this.min = min;
	}

	public String getMax() {
		return max;
	}

	public void setMax(String max) {
		this.max = max;
	}

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}

	public boolean hasMin() {
		return hasMinValue;
	}

	public void setHasMin(boolean hasMin) {
		this.hasMinValue = hasMin;
	}

	public boolean hasMax() {
		return hasMaxValue;
	}

	public void setHasMax(boolean hasMax) {
		this.hasMaxValue = hasMax;
	}

}
