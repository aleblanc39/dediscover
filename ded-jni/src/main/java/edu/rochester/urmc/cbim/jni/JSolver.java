package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.InterruptException;
import edu.rochester.urmc.cbim.jni.exception.OutOfMemoryException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;

public class JSolver extends Algorithm  {



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

//        System.err.println("Entering JSolver.solve");
//        JNIUtil.validateMemoryRequirements(estimatedMemoryUse(model));
//
//        JModelParameter modelParameters[] = model.getJModelParameters().toArray(new JModelParameter[0]);
//        JInitialCondition initialConditions[] = model.getJInitialConditions().toArray(new JInitialCondition[0]);
//
//        double modelParamValues[] = new double[modelParameters.length];
//        IntStream.range(0, modelParameters.length).forEach(i->{
//            modelParamValues[i] = estimableSymbolValues.get(modelParameters[i].getName());
//        });
//
//        double initialConditionValues[] = new double[initialConditions.length];
//        System.err.println("Jsolver.java setting initial conditions");
//        IntStream.range(0, initialConditions.length).forEach(i->{
//            initialConditionValues[i] = estimableSymbolValues.get(initialConditions[i].getName());
//        });
//
//        SolverResult solverResult;
//        if (timepoinsForSimulation != null && timepoinsForSimulation.size() > 0) {
//            double timepointsToUse[] = new double[timepoinsForSimulation.size()];
//            int cntr = 0;
//            for (Double d : timepoinsForSimulation) {
//                timepointsToUse[cntr++] = d.doubleValue();
//            }
//            solverResult = solveEqns(model, modelParamValues, initialConditionValues, timepointsToUse);
//        } else {
//            System.err.println("SOLVING FROM HERE");
//            solverResult = solveEqns(model, modelParamValues, initialConditionValues);
//        }
//        return solverResult;
//    }
//

    // private native methods


    private synchronized native SolverResult solveEqns(JDDEModel model, double modelParameterValues[],
                                                       double initialConditionValues[]) throws XDEException, InterruptException, OutOfMemoryException;

    private synchronized native SolverResult solveEqns(JDDEModel model, double modelParameterValues[],
                                                       double initialConditionValues[], double timepointsToUse[]) throws XDEException, InterruptException, OutOfMemoryException;

    public native long estimatedMemoryUse(JDDEModel model);

    public static native String[] s_getAvailableMethods();
    public static native boolean isAttributeDefined(String method, String attribute);

}
