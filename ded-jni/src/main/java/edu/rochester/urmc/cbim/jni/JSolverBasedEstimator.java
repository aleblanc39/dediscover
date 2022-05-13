package edu.rochester.urmc.cbim.jni;

import java.util.Arrays;
import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.XDEException;
import edu.rochester.urmc.cbim.jni.util.JNIUtil;

public class JSolverBasedEstimator extends JEstimator {

	public JSolverBasedEstimator(JDDEModel model,
								 JOptimizer optimizer,
								 JObjectiveFunction objectiveFunction,
								 JSolver solver,
								 List<Double> timepoints,
								 List<String> formulas,
								 List<List<Double>> columnData) {
		super(model, optimizer, objectiveFunction, solver);

		initializeEstimator(model,
				solver,
				objectiveFunction,
				optimizer,
				JNIUtil.rawDoubleArray(timepoints),
				formulas.toArray(new String[0]),
				JNIUtil.rawDoubleMatrix(columnData));
	}
	

	private native void initializeEstimator(JDDEModel model,
											JSolver solver,
											JObjectiveFunction objectiveFunction,
											JOptimizer optimizer,
											double[] timepoints,
											String[] formulas,
											double[][] data
											);
	
	public List<String>  getAvailableMethods() throws XDEException {
	    return Arrays.asList(s_getAvailableMethods());
	}

	private static native String [] s_getAvailableMethods();
	
}
