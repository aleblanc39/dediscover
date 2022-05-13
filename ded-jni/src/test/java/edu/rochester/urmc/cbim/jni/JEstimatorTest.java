package edu.rochester.urmc.cbim.jni;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;


import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.DoubleStream;
import java.util.stream.IntStream;

public class JEstimatorTest {
    static String equations =
            "dPrey/dt = (birth - predation * Predator) * Prey\n" +
                    "dPredator/dt = (growth * Prey - death) * Predator\n";


    int nbTimepoints = 50;
    JDDEModel model;
    JSolver solver;
    JOptimizer optimizer;
    JObjectiveFunction objfcn;
    JNIParameterSet paramSet;

    List<Double> timepoints = DoubleStream.iterate(0, n -> n + 1).limit(nbTimepoints).boxed().collect(Collectors.toList());
    List<String> formulas = Arrays.asList("Prey", "Predator");

    List<List<Double>> columnData;


    static {
        System.loadLibrary("DEDiscover");
    }


    @Before
    public void initialize() throws Exception {
        columnData = new ArrayList<>();
        model = new JDDEModel(equations);
        solver = Algorithm.createMethod(JSolver.class, JSolver.s_getAvailableMethods()[0]);
        paramSet = new JNIParameterSet(model);
        optimizer = Algorithm.createMethod(JOptimizer.class, Algorithm.getAvailableMethods(JOptimizer.class).get(0));
        objfcn = Algorithm.createMethod(JObjectiveFunction.class, JObjectiveFunction.s_getAvailableMethods()[0]);
        IntStream.range(0, formulas.size()).forEach(i ->
                columnData.add(DoubleStream.generate(() -> Math.random()).limit(50).boxed().collect(Collectors.toList())));
    }


//    @Test
//    public void testTestRun() {
//        System.out.println("Param set: " + paramSet.getNames().length);
//        JSolverBasedEstimator estimator = new JSolverBasedEstimator(model,
//                optimizer,
//                objfcn,
//                solver,
//                timepoints,
//                formulas,
//                columnData);
//
//        System.out.println("Created estimator");
//        estimator.setParameterSet(paramSet);
//
//        // Run estimation
//        try {
//            EstimatorResult er = estimator.estimate();
//            System.out.println("Creating CI object");
//            JConfidenceInterval jci = new JConfidenceInterval(JConfidenceInterval.s_getAvailableMethods()[0]);
//            jci.assignObjects(paramSet,
//                    er,
//                    model,
//                    solver,
//                    optimizer,
//                    timepoints,
//                    formulas,
//                    columnData);
//
//            System.out.println("Running CI");
//            EstimatorResult cir = jci.evaluate();
//            System.out.println("pvalues");
//            System.out.println(cir.getConfidenceIntervalResult().getPValues()[0]);
//            System.out.println(cir.getConfidenceIntervalResult().getPValues()[1]);
//
//            System.out.flush();
//
//
//        } catch (Exception ex) {
//            ex.printStackTrace();
//            fail();
//        }
//
//        // Run ci
//
//
//    }


    public void testGetResults() {
    }

    public void testEstimate() {


        JEstimator est = new JSolverBasedEstimator(model, optimizer, objfcn, solver, timepoints, formulas, columnData);

    }

    public void testSetParameterSet() {
    }

    public void testSetEstimateResults() {
    }
}