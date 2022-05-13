package edu.rochester.urmc.cbim.jni;

import org.junit.Before;
import org.junit.Test;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.DoubleStream;

import static org.junit.Assert.*;

public class JConfidenceIntervalTest {

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

    EstimatorResult estimatorResult;

    List<List<Double>> columnData;
    @Before
    public void initialize() {
    }

    static {
        System.loadLibrary("DEDiscover");
    }



    @Test
    public void run() {
    }

    @Test
    public void getNLSSummaryName() {
    }

    @Test
    public void evaluate() {
    }
}