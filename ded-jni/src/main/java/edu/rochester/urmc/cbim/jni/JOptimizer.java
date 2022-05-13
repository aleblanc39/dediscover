package edu.rochester.urmc.cbim.jni;


public class JOptimizer extends Algorithm {

    private JOptimizer() {
    }

    public JOptimizer(String optimizerName) {
        initializeOptimizer(optimizerName);
    }

    private native void initializeOptimizer(String name);

    public static native String[] s_getAvailableMethods();
    public static native boolean isAttributeDefined(String method, String attribute);

}
