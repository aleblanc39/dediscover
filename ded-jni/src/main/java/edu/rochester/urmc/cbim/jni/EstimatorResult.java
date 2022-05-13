package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.util.CppPeer;

public class EstimatorResult extends CppPeer {

    private EstimatorResult(){
	
    }

    public native double getOptimalValue();
    public native double getRSS();
    public native double getAIC();
    public native double getAICc();
    public native double getBIC();

    public native int getNObs();
    public native double[][] getIntermediateResults();
    public native double[] getBestResult();
    public native String[] getEstimatedParameterNames();

}
