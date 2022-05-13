package edu.rochester.urmc.cbim.jni;

import edu.rochester.urmc.cbim.jni.exception.XDEException;
import org.apache.commons.math3.util.Pair;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;


public class JDDEModel extends Threadable {

    List<JMacro> macros = new ArrayList<>();
    List<JCovariate> covariates = new ArrayList<>();
    List<JModelParameter> modelParameters = new ArrayList<>();
    List<JInitialCondition> variables = new ArrayList<>();



    /**
     * Constructor where the equations string is passed as parameter.
     */


    public JDDEModel(String equations) {
            createModel(equations);
            initLocalParams();
    }


    /**
     * Compute the macro AND covariate values at the timepoints given in the argument.
     *
     * @param timepts
     * @param depVarValues
     * @param modelParamValues
     * @return
     * @throws XDEException
     */
    public JXDECSVFile computeMacroValues(double[] timepts,
                                          double[][] depVarValues,
                                          double[] modelParamValues,
                                          Map<String, List<Pair<Double, Double>>> tvfInitializationPoints) throws XDEException {

        covariates.forEach(cov -> {
            if (tvfInitializationPoints.keySet().contains(cov.getName())) {
                cov.setTVFMapped(true);
                cov.getTimeVaryingFunction().addControlPoints(tvfInitializationPoints.get(cov.getName()));
            } else {
                cov.setTVFMapped(false);
            }
        });

        return computeMacroValues_nat(timepts, depVarValues, modelParamValues, timepts[0], timepts[timepts.length - 1]);
    }

    private native JXDECSVFile computeMacroValues_nat(double[] timepts, double[][] depVarValues,
                                                      double[] modelParameterValues, double t0, double tend) throws XDEException;

    // Compute formulas. The column names on return with be the formula names.
    public native JXDECSVFile computeFormulas_nat(double[] timepts, double[][] depVarValues, double[] modelParameterValues,
                                                  String [] formulas);


    private native void createModel(String model) throws XDEException;

    // Error messages in C++ is a vector pairs<line_no, msg> where the int is the line #.
    // Converting pairs is too hard. Will be converted into strings of the form line_no:msg.

    public List<Pair<Integer, String>> getErrorMessages() {
        List<Pair<Integer, String>> ret = new ArrayList<>();
        Arrays.stream(nat_getErrorMessages()).collect(Collectors.toList()).forEach(err -> {
                    int sep = err.indexOf(':');
                    ret.add(new Pair(Integer.parseInt(err.substring(0, sep)), err.substring(sep+1)));
                });
        return ret;
    }
    private native  String [] nat_getErrorMessages();
    public native String[] validateFormula(String formula);


    public List<String> getDependentVariableNames() {
        List<JInitialCondition> init = getJInitialConditions();
        return getSymbolNames(init);
    }

    private List<String> getSymbolNames(List<? extends JModelSymbol> ms) {
        List<String> names = new ArrayList<>(ms.size());
        for (JModelSymbol s : ms) {
            names.add(s.getName());
        }
        return names;
    }

    public List<JInitialCondition> getJInitialConditions() {
        return variables;
    }

    /**
     * Get a list of the macros in the model.
     *
     * @return
     */
    public List<JMacro> getJMacros() {
        return macros;
    }
    public List<JCovariate> getJCovariates() {
        return covariates;
    }
    public List<JModelParameter> getJModelParameters() {
        return modelParameters;
    }

    private native JMacro[] getMacros();
    private native JInitialCondition[] getInitialConditions();
    private native JModelParameter[] getModelParameters();
    private native JCovariate[] getCovariates();

    public native int getNbDelayExpressions() throws XDEException;

    /**
     * Save a local handle to the list of variables, macros, and parameters to
     * avoid excessive copying from C-land
     */
    private void initLocalParams() {
        modelParameters = Arrays.asList(getModelParameters());

        macros = Arrays.asList(getMacros());
        covariates = Arrays.asList(getCovariates());
        variables = Arrays.asList(getInitialConditions());
    }


    public void updateJEquations(String equations) {
        createModel(equations);
        initLocalParams();
    }
}
