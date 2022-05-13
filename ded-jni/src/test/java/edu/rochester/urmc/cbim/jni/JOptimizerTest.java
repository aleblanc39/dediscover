package edu.rochester.urmc.cbim.jni;

import org.junit.Test;

import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.*;

public class JOptimizerTest {
    static {
        System.loadLibrary("DEDiscover");
    }

    @Test
    public void testAvailableMethods() throws Exception {
        List<String> methods = Algorithm.getAvailableMethods(JOptimizer.class);
        for (String s: methods) {
            System.out.println(s);
        }
    }

    @Test
    public void testMethodCreation() throws Exception {
        List<String> methods = Algorithm.getAvailableMethods(JOptimizer.class);
        for (String s: methods) {
            Object x = JOptimizer.createMethod(JOptimizer.class, s);
            System.out.println("Created for " + s + " and didn't crash");

        }
    }


}