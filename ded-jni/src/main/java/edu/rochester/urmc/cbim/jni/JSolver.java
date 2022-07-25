package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.InterruptException;
import edu.rochester.urmc.cbim.jni.exception.OutOfMemoryException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;
import org.apache.commons.math3.util.Pair;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class JSolver extends Algorithm  {

    public JSolver(String solverName, JDDEModel model, HashMap<String, String> controlParameterValues, HashMap<String, JTimeVaryingFunction> covariates) {



    }

    public static SolverResult solve()  {
            return null;
        }
        /**
         * First estimate the amount of memory required, and how much is
         * available. Throw an exception if it looks like there will not be
         * enough memory throw an exception.
         *
         */

//        public static SolverResult solve(JDDEModel model, Map<String, Double> estimableSymbolValues, List<Double> timepoinsForSimulation){

    private native void createSolver(String solverName, ) throws XDEException;

    private synchronized native SolverResult solveEqns(JDDEModel model, double modelParameterValues[],
                                                       double initialConditionValues[]) throws XDEException, InterruptException, OutOfMemoryException;

    private synchronized native SolverResult solveEqns(JDDEModel model, double modelParameterValues[],
                                                       double initialConditionValues[], double timepointsToUse[]) throws XDEException, InterruptException, OutOfMemoryException;

    public static native String[] s_getAvailableMethods();
    public static native boolean isAttributeDefined(String method, String attribute);

}
