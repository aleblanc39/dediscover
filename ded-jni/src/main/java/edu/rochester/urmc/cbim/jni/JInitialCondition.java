
package edu.rochester.urmc.cbim.jni;

public class JInitialCondition extends JModelSymbol {

    private JInitialCondition(){
	
    }
	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.rochester.urmc.cbim.jni.JModelSymbol#getDisplayName()
	 */
	@Override
	public String getDisplayName() {
		return super.getDisplayName() + "[t0]";
	}

	public native String getFormula();
}
