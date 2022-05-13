
package edu.rochester.urmc.cbim.jni;

import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.InterruptException;
import edu.rochester.urmc.cbim.jni.exception.OutOfMemoryException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;
import edu.rochester.urmc.cbim.jni.exception.XDESystemException;

public abstract class JEstimator extends Algorithm  {

    final JDDEModel model;
    public final JOptimizer optimizer;
    final JObjectiveFunction objectiveFunction;
    final JSolver solver;

    public JEstimator(
             JDDEModel model,
             JOptimizer optimizer,
             JObjectiveFunction objectiveFunction,
             JSolver solver

    ) {
        this.model = model;
        this.optimizer = optimizer;
        this.objectiveFunction = objectiveFunction;
        this.solver = solver;
    }



    //@Override
    public EstimatorResult runEstimation() throws InterruptException, OutOfMemoryException {

        if (parameterSet == null) {
            throw new XDESystemException(
                    "Attempting to perform estimation without defining a parameter set.");
        }
        try {
            return  estimate();

        } catch (InterruptException e) {
            System.err.println("Estimation cancelled by user");
            throw e;
        } catch (Exception e) {

            e.printStackTrace();
            throw e;
        }
    }

    public EstimatorResult estimate() throws XDEException, InterruptException,
            OutOfMemoryException {
        List<ParameterValue> values = parameterSet.getParameters();
        return nat_estimate(values.toArray(new ParameterValue[values.size()]));
    }

    private native EstimatorResult nat_estimate(ParameterValue[] params) throws XDEException, InterruptException,
            OutOfMemoryException;

    public void setParameterSet(JNIParameterSet ps) {
        parameterSet = ps;
    }

    private JNIParameterSet parameterSet = null;

}
