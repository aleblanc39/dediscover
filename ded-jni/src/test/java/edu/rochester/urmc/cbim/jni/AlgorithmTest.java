package edu.rochester.urmc.cbim.jni;

import junit.framework.TestCase;

import java.util.List;

public class AlgorithmTest extends TestCase {

    static {
        System.loadLibrary("DEDiscover");
    }

    public void testGetAvailableMethods() {
        try {
            List<String> methods = Algorithm.getAvailableMethods(JSolver.class);
            methods.forEach(System.out::println);

            methods = Algorithm.getAvailableMethods(JObjectiveFunction.class);
            methods.forEach(System.out::println);

            methods = Algorithm.getAvailableMethods(JOptimizer.class);
            methods.forEach(System.out::println);

            methods = Algorithm.getAvailableMethods(JTimeVaryingFunction.class);
            methods.forEach(System.out::println);

        } catch (Exception ex) {
            ex.printStackTrace();
            fail();
        }

    }

    public void testCreateMethod() {
    }
}