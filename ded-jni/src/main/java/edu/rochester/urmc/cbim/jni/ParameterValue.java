
package edu.rochester.urmc.cbim.jni;


import edu.rochester.urmc.cbim.jni.exception.ParameterRangeException;
public class ParameterValue {

	/**
	 * 
	 */
	private static final String NAN_STRING = String.valueOf(Double.NaN);
	public final String name;
	public final double min, value, max;
	public final boolean estimated;

	public ParameterValue(String name, double min, double max, double value,
			boolean estimated, boolean relaxed) {
		if (!relaxed && !isValid(min, max, value)) {
			throw new ParameterRangeException(name, min, value, max);
		}
		if (NAN_STRING.equals(String.valueOf(min))
				|| NAN_STRING.equals(String.valueOf(max))
				|| NAN_STRING.equals(String.valueOf(value))) {
			min = 0d;
			value = 0d;
			max = 1d;
		}
		this.name = name;
		this.min = min;
		this.max = max;
		this.value = value;
		this.estimated = estimated;
	}


	public ParameterValue(JModelSymbol symbol) {
		this(symbol.getName(), 0.0, 1.0, 0.5, true, false);
	}

	public ParameterValue(String name, double min, double max, double value,
			boolean estimated) {
		this(name, min, max, value, estimated, false);
	}

	protected boolean isValid(double min, double max, double value) {
		return (value >= min) && (value <= max);
	}


	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "ParameterValue [name=" + name + ", min=" + min + ", value="
				+ value + ", max=" + max + ", estimated=" + estimated
				+ ", slider="  + "]";
	}

}
