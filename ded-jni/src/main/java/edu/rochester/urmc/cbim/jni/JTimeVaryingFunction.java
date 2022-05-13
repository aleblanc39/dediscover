package edu.rochester.urmc.cbim.jni;

import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.XDEException;
import org.apache.commons.math3.util.Pair;

public class JTimeVaryingFunction extends Algorithm {


     public JTimeVaryingFunction() {
        createTimeVaryingFunction(null);
    }

    public JTimeVaryingFunction(String functionName) {
        createTimeVaryingFunction(functionName);
    }


    public void addControlPoints(List<Pair<Double, Double>> points) {
        double[][] d = createControlPointsMatrix(points);
        nat_addControlPoints(d);

    }

    public native void nat_setControlPoints(double[][] points);
    public native void nat_addControlPoints(double[][] points);
    public native double compute(double v);
    public native void nat_initialize();

    private native void createTimeVaryingFunction(String functionName)
            throws XDEException;



    private double[][] createControlPointsMatrix(List<Pair<Double, Double> >points) {
        double[][] pointsArray = new double[points.size()][2];

        int cntr = 0;
        for (Pair<Double, Double> pt : points) {
            pointsArray[cntr][0] = pt.getFirst();
            pointsArray[cntr][1] = pt.getSecond();
            cntr++;
        }
        return pointsArray;
    }

    public static native String[] s_getAvailableMethods();

}
