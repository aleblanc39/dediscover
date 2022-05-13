package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.XDEException;

public class JIntermediateSymbol extends XDEBase implements
	MappableSymbol {


	public native String getName();

	public native String getDescription();

	public native String getFormula() throws XDEException;
	public native String getDisplayExpression() throws XDEException;
	
	public native void setDescription(String desc) throws XDEException;


}
