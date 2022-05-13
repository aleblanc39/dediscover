
package edu.rochester.urmc.cbim.jni;

import java.util.ArrayList;
import java.util.List;

import edu.rochester.urmc.cbim.jni.util.CppPeer;

public class JConfidenceIntervalResult extends CppPeer {

	public class CIResultObject {

		public final double lowerBound;

		public final String name;
		public final double pvalue;
		public final double standardError;
		public final double tstat;
		public final double upperBound;
		public final double value;

		protected CIResultObject(String name, double value,
				double standardError, double lowerBound, double upperBound,
				double tstat, double pvalue) {
			this.name = name;
			this.value = value;
			this.standardError = standardError;
			this.lowerBound = lowerBound;
			this.upperBound = upperBound;
			this.tstat = tstat;
			this.pvalue = pvalue;
		}
	}

	public JConfidenceIntervalResult() {
	}

	public native double[] getLowerBounds();

	public native String[] getParameterNames();

	public native double[] getParameterValues();

	public native double[] getPValues();

	public List<CIResultObject> getResultObjects() {
		List<CIResultObject> ret = new ArrayList<>();

		double[] parameterValues = getParameterValues();
		double[] standardError = getStandardErrors();
		double[] lowerBound = getLowerBounds();
		double[] upperBound = getUpperBounds();
		double[] tstat = getTStat();
		double[] pvalues = getPValues();
		String[] names = getParameterNames();

		for (int i = 0; i < names.length; i++) {
			ret.add(new CIResultObject(names[i], parameterValues[i],
					standardError[i], lowerBound[i], upperBound[i], tstat[i],
					pvalues[i]));
		}
		return ret;

	}

	public native double[] getStandardErrors();

	public native double[] getTStat();

	public native double[] getUpperBounds();
}
