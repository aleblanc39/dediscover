package edu.rochester.urmc.cbim.jni;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import edu.rochester.urmc.cbim.jni.exception.XDEException;

public class JObjectiveFunction extends Algorithm {


    public static final String LOG_PARAMETER_NAME = "Log values";
    public static final String SCALING_PARAMETER_NAME = "Normalize data";

    private static List<String> availableMethods = new ArrayList<String>(5);


    // Constructors still needed because objects may be created in JNI
    private JObjectiveFunction() {
    }
	public JObjectiveFunction(String functionName) throws XDEException {
		initializeObject(functionName);
	}


    private native void initializeObject(String functionName)
            throws XDEException;

    public static native String[] s_getAvailableMethods();
    public static native boolean isAttributeDefined(String method, String attribute);

    static {
        availableMethods.addAll(Arrays.asList(s_getAvailableMethods()));
    }
}
