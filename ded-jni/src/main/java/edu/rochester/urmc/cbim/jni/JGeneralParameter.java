
package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.ParameterRangeException;
import edu.rochester.urmc.cbim.jni.exception.XDEException;

/**
 * Class to handle general parameters.
 *
 * This class will handle the general parameter. Each object with this
 * class will contain a DoubleParameter, StringParameter, IntParameter
 * or BooleanParameter C++ object, or an object from any further
 * classes derived from GeneralParameter.
 *
 * There will be only one Java class defined to handle are the classes
 * derived from General parameter. The value type will be Object, and
 * the user of this class can use the Java type identification
 * facilities to determine the type of the data.
 */

public class JGeneralParameter extends XDEBase {
    

    public native void setDescription(String s) throws XDEException;

    public native String getName() throws XDEException;
    public native String getDescription() throws XDEException;

    public native Object getValue() throws XDEException;
    public native void setValue(Object o) throws ParameterRangeException, XDEException;

    public native Object getMin();
    public native Object getMax();
    public native Object getDefault();

    public native boolean hasMin();
    public native boolean hasMax();

    // Methods to handle parameter with multiple values.
    public native boolean hasOptions();
    public native int nbOptionsRequired();
    //public native void setValues(Object newValues[]);
    public native Object [] getOptions();

    private JGeneralParameter() {
   }
    

}


    
