
package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.util.CppPeer;

public class JXDECSVFile extends CppPeer {


    private JXDECSVFile() {
    }

    public JXDECSVFile(String[] varNames, double[] timepoints, double[][] data) {
        createNativeObject();
        setVariableNames(varNames);
        setTimepoints(timepoints);
        setData(data);
    }

    public native void setTimepoints(double[] t);

    public native void setData(double[][] d);

    public native void setVariableNames(String[] names);

    public  double[] getjTimepoints(){return getTimepoints();}
    public native double[] getTimepoints();

    public double[][] getjData() { return getData();}
    public native double[][] getData();

    public  String[] getjVariableNames(){return getVariableNames();};
    public native String[] getVariableNames();

    // Some functions to get info on the data without having to create every
    // time.


    public  int getjNbTimepoints(){return getNbTimepoints();};
    public native int getNbTimepoints();

    public  int getjNbVariables(){return getNbVariables();};
    public native int getNbVariables();

    private native void createNativeObject();

}
