package edu.rochester.urmc.cbim.jni;

import org.junit.Before;
import org.junit.Test;


import java.util.stream.IntStream;

import static org.junit.Assert.*;


public class JSolverTest {

    @Before
    public void initialize() {

    }

    static {
        System.loadLibrary("DEDiscover");
    }




    @Test
    public void getMethodName() {
    }

    @Test
    public void getShortMethodDescription() {
    }

    @Test
    public void getLongMethodDescription() {
        assertEquals(1, 1);
    }

    @Test
    public void getMethodAttributes() {
    }

    @Test
    public void getControlParameter() {
    }

    @Test
    public void getControlParameters() {
    }

    @Test
    public void s_getAvailableMethods() {
        String[] methods = JSolver.s_getAvailableMethods();
        assertTrue(methods.length > 0);

    }

    @Test
    public void isAttributeDefined() {
        // Will test that all the solver are actually of type DDE or ODE
        // Technically not really a unit test on this particular class, still useful.
        String[] methods = JSolver.s_getAvailableMethods();
        for (String s: methods) {
            assertNotEquals(JSolver.isAttributeDefined(s, "ODE"), JSolver.isAttributeDefined(s, "DDE"));
        }
    }
}