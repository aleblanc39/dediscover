
package edu.rochester.urmc.cbim.jni;

import java.util.*;
import java.util.function.Function;
import java.util.stream.Collectors;

/**
 * Main purpose of this class is to take the estimable parameters from the GUI and pass the values to the JNI
 * for estimation.
 *
 * On creation will create default params for all the model params and initial conditions, then they can be replaced
 * as needed.
 *
 * The lifetime of an object of this class should be short: created when an estimation is about to start, freed
 * once the estimation is completed.
 */

public class JNIParameterSet  {

	Map<String, ParameterValue> parameters = new HashMap();
	Function<JModelSymbol, ParameterValue> addToMap =(JModelSymbol m) -> parameters.put(m.getName(), new ParameterValue(m));

	public JNIParameterSet() {
	}

	public JNIParameterSet(JDDEModel model) {
		model.modelParameters.forEach(p->addToMap.apply(p));
		model.variables.forEach(p->addToMap.apply(p));
	}

	public List<ParameterValue> getParameters() {
		return parameters.values().stream().collect(Collectors.toList());

	}
	public String[] getNames() {
		return parameters.keySet().toArray(new String[parameters.size()]);
	}

	/**
	 * Add or replace a parameter value.
	 * @param pv
	 */
	public void setParameterValues(ParameterValue pv) {
		parameters.put(pv.name, pv);
	}

}
