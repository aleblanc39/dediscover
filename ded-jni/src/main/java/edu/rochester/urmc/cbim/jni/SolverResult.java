
package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.util.CppPeer;

public class SolverResult extends CppPeer {

	JXDECSVFile data = null;

	public native double[] getTimepoints();
	public native double[][] getResults();
	public native String[] getVariableNames();
	public native int getNbVariables();
	public native int getNbTimepoints();


	/**
	 * AL 2020 Adding java method that wrap around the native methods in order to allow mocking.
	 * All mock packages I found have issue with native methods
	 */

	public  double[] jgetTimepoints() {return getTimepoints();}
	public  double[][] jgetResults() {return getResults();}
	public  String[] jgetVariableNames() {return getVariableNames();}
	public  int jgetNbVariables() {return getNbVariables();}
	public  int jgetNbTimepoints() {return getNbTimepoints();}

	/**
	 * Creates an object containing only the data so it can be manipulated by
	 * the caller. Will not change the actual result. The data is cached. If the
	 * caller changes it and calls this method again it will get the changed
	 * results.
	 * 
	 * 
	 */
	public JXDECSVFile getData() {
		if (data == null) {
			data = new JXDECSVFile(getVariableNames(), getTimepoints(),
					getResults());
		}
		return data;
	}

	private SolverResult() {
	}


}
