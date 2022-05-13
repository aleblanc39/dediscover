package edu.rochester.urmc.cbim.jni;

public class JCovariate extends JIntermediateSymbol {

    
	private JCovariate(){
	    
	}
	public native boolean isMapped();
	public native void setTVFMapped(boolean b);
	public native JTimeVaryingFunction getTimeVaryingFunction();
	public native void setTimeVaryingFunction(JTimeVaryingFunction tvf);

}
