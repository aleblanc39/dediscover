package edu.rochester.urmc.cbim.jni;

import org.apache.commons.math3.util.Pair;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class JTimeVaryingFunctionTest {

    static {
        System.loadLibrary("DEDiscover");
    }

    List<Pair<Double, Double>> controlPoints = new ArrayList<Pair<Double, Double>>(
            Arrays.asList(
                    new Pair<Double, Double>(0.0, 0.0),
                    new Pair<Double, Double>(1.0, 6.0),
                    new Pair<Double, Double>(2.0, 3.0),
                    new Pair<Double, Double>(4.0, 11.0)
            )

    );

    @Test
    public void TestLOCF() {
        JTimeVaryingFunction tvf = new JTimeVaryingFunction("LOCF");

        tvf.addControlPoints(controlPoints);
        tvf.nat_initialize();
        for (double d = 0; d < 10; d+=0.3) {
            System.err.println("" + d + ": " + tvf.compute(d));
        }
    }
}
