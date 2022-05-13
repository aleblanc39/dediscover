
package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.InterruptException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;
import edu.rochester.urmc.cbim.jni.exception.XDESystemException;
import edu.rochester.urmc.cbim.jni.util.JNIUtil;

import java.util.List;

public class JConfidenceInterval extends Algorithm {

     JDDEModel model;
     JOptimizer optimizer;
     JSolver solver;
     List<Double> timepoints;
     List<String> formulas;
     List<List<Double>> columnData;
     JNIParameterSet parameterSet;
     EstimatorResult estimatorResult;


    public JConfidenceInterval(String name
                               ) {
        initializeMethod(name);

    }


    public void assignObjects(JNIParameterSet parameterSet,
                                      EstimatorResult estimatorResult,
                                      JDDEModel model,
                                      JSolver solver,
                                      JOptimizer optimizer,
                                      List<Double> timepoints,
                                      List<String> formulas,
                                      List<List<Double>> columnData) {
        this.model = model;
        this.solver = solver;
        this.optimizer = optimizer;
        this.timepoints = timepoints;
        this.formulas = formulas;
        this.columnData = columnData;
        this.parameterSet = parameterSet;
        this.estimatorResult = estimatorResult;
    }

    private native void initializeMethod(String name);


    //@Override
    public EstimatorResult run() throws InterruptException {
        if (parameterSet == null) {
            throw new XDESystemException(
                    "Attempting to perform CI computation without defining a parameter set.");
        }


        try {
            return evaluate();
        } catch (XDEException e) {
            e.printStackTrace();
            throw e;
        } catch (InterruptException e) {
            System.err.println("Got message to interrupt the thread");
            throw e;
        } catch (Exception e) {
            e.printStackTrace();
            throw  e;
        }

    }
    public static native String[] s_getAvailableMethods();

    private EstimatorResult evaluate()
            throws XDEException, InterruptException {
        List<ParameterValue> values =  parameterSet.getParameters();

        EstimatorResult ret = nat_evaluate(
                values.toArray(new ParameterValue[values.size()]),
                estimatorResult,
                model,
                solver,
                optimizer,
                JNIUtil.rawDoubleArray(timepoints),
                formulas.toArray(new String[0]),
                JNIUtil.rawDoubleMatrix(columnData)
                );

        return ret;
    }

    private native EstimatorResult nat_evaluate(
            ParameterValue[] params,
            EstimatorResult er,
            JDDEModel model,
            JSolver solver,
            JOptimizer optimizer,
            double[] timepoints,
            String [] symbolNames,
            double[][] data)  throws XDEException, InterruptException;
}
